#include "pch.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "D3D11/ContextUtils.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Viewport.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Resources/VertexBuffer.hpp"
#include "D3D11/Resources/MappedBufferHandle.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/Renderer.hpp"

using namespace Weave;
using namespace Weave::D3D11;

CtxBase::CtxBase() = default;

CtxBase::CtxBase(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx) :
	DeviceChild(dev),
	pCtx(std::move(pCtx)),
	pState(new ContextState()),
	isImmediate(this->pCtx->GetType() == D3D11_DEVICE_CONTEXT_IMMEDIATE)
{
	pState->Init();
}

CtxBase::CtxBase(CtxBase&&) noexcept = default;

CtxBase& CtxBase::operator=(CtxBase&&) noexcept = default;

void CtxBase::BindDepthStencilBuffer(IDepthStencil& depthStencil) 
{
	if (pState->pDepthStencil != &depthStencil)
	{
		pState->pDepthStencil = &depthStencil;
		pCtx->OMSetDepthStencilState(depthStencil.GetState(), 1u);
		pCtx->OMSetRenderTargets(pState->rtCount, pState->rtvs.GetData(), depthStencil.GetDSV());
	}
}

void CtxBase::UnbindDepthStencilBuffer() 
{
	if (pState->pDepthStencil != nullptr)
	{
		pState->pDepthStencil = nullptr;
		pCtx->OMSetDepthStencilState(nullptr, 1);
		pCtx->OMSetRenderTargets(pState->rtCount, pState->rtvs.GetData(), nullptr);
	}
}

uint CtxBase::GetRenderTargetCount() const { return pState->rtCount; }

void CtxBase::BindRenderTarget(IRenderTarget& rt, IDepthStencil& ds, sint slot)
{
	Span rtSpan(&rt);
	BindRenderTargets(rtSpan, &ds, slot);
}

void CtxBase::BindRenderTarget(IRenderTarget& rt, sint slot)
{
	Span rtSpan(&rt);
	BindRenderTargets(rtSpan, slot);
}

/// <summary>
/// Updates RTV state cache and returns true if any changes were made
/// </summary>
static bool TryUpdateRTV(ContextState& state, IDynamicArray<IRenderTarget>& rts, sint offset)
{
	bool wasChanged = false;
	const uint newCount = (uint)rts.GetLength();

	if ((offset + newCount) > state.rtCount)
	{
		state.rtCount = offset + newCount;
		wasChanged = true;
	}

	Span rtvState(&state.rtvs[offset], newCount);

	for (uint i = 0; i < newCount; i++)
	{
		ID3D11RenderTargetView* pRTV = rts[i].GetRTV();

		if (pRTV != rtvState[i])
		{
			rtvState[i] = pRTV;
			wasChanged = true;
		}
	}

	return wasChanged;
}

static bool TryUpdateViewports(ContextState& state, const IDynamicArray<IRenderTarget>& rts, sint offset)
{
	// Update viewports for render textures
	Span vpState(&state.viewports[offset], rts.GetLength());
	const vec2 depthRange = state.pDepthStencil != nullptr ? state.pDepthStencil->GetRange() : vec2(0);
	const uint newCount = (uint)rts.GetLength();
	bool wasChanged = false;

	for (uint i = 0; i < newCount; i++)
	{
		Viewport vp
		{
			.offset = rts[i].GetRenderOffset(),
			.size = rts[i].GetRenderSize(),
			.zDepth = depthRange
		};

		if (vp != vpState[i])
		{
			vpState[i] = vp;
			wasChanged = true;
		}
	}

	state.vpCount = std::max(state.vpCount, (uint)(offset + newCount));
	return wasChanged;
}

void CtxBase::BindRenderTargets(IDynamicArray<IRenderTarget>& rts, sint startSlot)
{
	D3D_ASSERT_MSG(rts.GetLength() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"Number of render targets supplied exceeds limit.");

	if (TryUpdateRTV(*pState, rts, startSlot))
		pCtx->OMSetRenderTargets(pState->rtCount, pState->rtvs.GetData(), pState->GetDepthStencilView());

	if (TryUpdateViewports(*pState, rts, startSlot))
		pCtx->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
}

void CtxBase::BindRenderTargets(IDynamicArray<IRenderTarget>& rts, IDepthStencil* pDS, sint startSlot)
{
	D3D_ASSERT_MSG(rts.GetLength() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"Number of render targets supplied exceeds limit.");

	bool wasDsChanged = false;

	if (pDS != pState->pDepthStencil)
	{
		ID3D11DepthStencilState* pDSS = pDS != nullptr ? pDS->GetState() : nullptr;
		pState->pDepthStencil = pDS;
		pCtx->OMSetDepthStencilState(pDSS, 1);
		wasDsChanged = true;
	}

	const bool wasRtvChanged = TryUpdateRTV(*pState, rts, startSlot);

	if (wasDsChanged || wasRtvChanged)
		pCtx->OMSetRenderTargets(pState->rtCount, pState->rtvs.GetData(), pState->GetDepthStencilView());
		
	if (TryUpdateViewports(*pState, rts, startSlot))
		pCtx->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
}

void CtxBase::UnbindRenderTarget(sint slot)
{
	UnbindRenderTargets(slot, 1);
}

void CtxBase::UnbindRenderTargets(sint startSlot, uint count)
{
	if (pState->rtCount == 0 || count == 0)
		return;

	if (count == (uint)-1)
	{
		startSlot = 0;
		count = pState->rtCount;
	}

	SetArrNull(pState->rtvs, startSlot, count);
	
	if ((startSlot + count) == pState->rtCount)
		pState->rtCount = startSlot;

	if (pState->rtCount == 0)
	{
		pState->pDepthStencil = nullptr;
		pCtx->OMSetDepthStencilState(nullptr, 1u);
		pCtx->OMSetRenderTargets(0, nullptr, nullptr);
	}
	else
		pCtx->OMSetRenderTargets(startSlot, pState->rtvs.GetData(), pState->GetDepthStencilView());
}

void CtxBase::SetPrimitiveTopology(PrimTopology topology)
{
	if (topology != pState->topology)
	{
		pState->topology = topology;
		pCtx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)topology);
	}
}

void CtxBase::BindVertexBuffer(VertexBuffer& vertBuffer, sint slot)
{
	Span vbSpan(&vertBuffer);
	BindVertexBuffers(vbSpan, slot);
}

void CtxBase::BindVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, sint startSlot)
{
	Span vbState(pState->vertexBuffers.GetData() + startSlot, vertBuffers.GetLength());
	const uint newCount = (uint)vertBuffers.GetLength();
	bool wasChanged = false;

	for (uint i = 0; i < newCount; i++)
	{
		ID3D11Buffer* pVB = vertBuffers[i].Get();

		if (pVB != vbState[i])
		{
			vbState[i] = pVB;
			wasChanged = true;
		}
	}

	if (wasChanged)
	{
		Span strideState(pState->vbStrides.GetData() + startSlot, newCount);
		Span offsetState(pState->vbOffsets.GetData() + startSlot, newCount);
		pState->vertBufCount = std::max(pState->vertBufCount, (uint)(startSlot + newCount));

		for (uint i = 0; i < newCount; i++)
		{
			strideState[i] = vertBuffers[i].GetStride();
			offsetState[i] = vertBuffers[i].GetOffset();
		}

		pCtx->IASetVertexBuffers(startSlot, newCount, vbState.GetData(), strideState.GetData(), offsetState.GetData());
	}
}

void CtxBase::UnbindVertexBuffer(sint slot)
{
	UnbindVertexBuffers(slot, 1);
}

void CtxBase::UnbindVertexBuffers(sint startSlot, uint count)
{
	if (pState->vertBufCount == 0 || count == 0)
		return;

	if (count == (uint)-1)
	{
		startSlot = 0u;
		count = pState->vertBufCount;
	}

	SetArrNull(pState->vertexBuffers, startSlot, count);
	SetArrNull(pState->vbStrides, startSlot, count);
	SetArrNull(pState->vbOffsets, startSlot, count);
	
	if ((startSlot + count) == pState->vertBufCount)
		pState->vertBufCount = startSlot;

	pCtx->IASetVertexBuffers(startSlot, count, pState->vertexBuffers.GetData(), pState->vbStrides.GetData(), pState->vbOffsets.GetData());
}

void CtxBase::BindIndexBuffer(IndexBuffer& indexBuffer, uint byteOffset)
{
	if (pState->pIndexBuffer != indexBuffer.Get())
	{
		pState->pIndexBuffer = indexBuffer.Get();
		pCtx->IASetIndexBuffer(pState->pIndexBuffer, (DXGI_FORMAT)indexBuffer.GetFormat(), byteOffset);
	}
}

void CtxBase::UnbindIndexBuffer()
{
	if (pState->pIndexBuffer != nullptr)
	{
		pState->pIndexBuffer = nullptr;
		pCtx->IASetIndexBuffer(nullptr, (DXGI_FORMAT)Formats::UNKNOWN, 0);
	}
}

void CtxBase::ClearRenderTarget(IRenderTarget& rt, vec4 color)
{
	pCtx->ClearRenderTargetView(rt.GetRTV(), reinterpret_cast<float*>(&color));
}

bool CtxBase::GetIsBound(const ShaderVariantBase& shader) const
{
	const ShadeStages stage = shader.GetStage();
	return pState->GetStage(stage).pShader == &shader;
}

void CtxBase::HandleConflicts()
{
	const ContextState::ConflictState conflicts = pState->TryResolveConflicts();
	
	// This currently allows all conflicts to be set to null without raising an error. For view state 
	// transitions, this is appropriate. For other cases, it is not.
	if (!conflicts.IsEmpty())
	{
		for (uint i = 0; i < g_ShadeStageCount; i++)
		{
			const ShadeStages stage = (ShadeStages)i;
			const ContextState::StageState& ss = pState->GetStage(stage);

			if (conflicts.srvsExtent[i] > 0)
				SetShaderResources(pCtx.Get(), stage, 0, conflicts.srvsExtent[i], ss.srvs.GetData());
		}

		if (conflicts.uavsExtent > 0)
			CSSetUnorderedAccessViews(pCtx.Get(), 0, conflicts.uavsExtent, pState->uavs.GetData());

		if (conflicts.rtvExtent > 0)
			pCtx->OMSetRenderTargets(conflicts.rtvExtent, pState->rtvs.GetData(), pState->GetDepthStencilView());
	}
}

void CtxBase::BindResources(const ComputeShaderVariant& shader, const ResourceSet& resSrc)
{
	if (const uint updateCount = pState->TryUpdateResources(resSrc.GetUAVs(), shader.GetUAVMap()); updateCount > 0)
	{
		HandleConflicts();
		CSSetUnorderedAccessViews(pCtx.Get(), 0, updateCount, pState->uavs.GetData());
	}
}

void CtxBase::BindResources(const VertexShaderVariant& shader, const ResourceSet& resSrc)
{
	ID3D11InputLayout* pLayout = shader.GetInputLayout().Get();
	
	if (pLayout != pState->pInputLayout)
	{
		pState->pInputLayout = pLayout;
		pCtx->IASetInputLayout(pState->pInputLayout);
	}
}

void CtxBase::BindShader(const ShaderVariantBase& shader, const ResourceSet& resSrc)
{
	const ShadeStages stage = shader.GetStage();
	const ContextState::StageState& ss = pState->GetStage(stage);

	// Bind shader
	if (ss.pShader != &shader)
		SetShader(pCtx.Get(), stage, shader.Get(), nullptr, 0);

	// Upload constants
	if (shader.GetConstantMap() != nullptr && resSrc.GetConstantCount() > 0)
	{
		const ConstantGroupMap& constMap = *shader.GetConstantMap();
		const ConstantGroupMap::Data& constSrc = resSrc.GetMappedConstants(constMap);
		IDynamicArray<ConstantBuffer>& cbufs = shader.GetConstantBuffers();

		// Upload data to each ConstantBuffer object
		for (uint i = 0; i < constMap.GetBufferCount(); i++)
			SetBufferData(cbufs[i], constSrc[i]);
	}

	// Bind constant buffers
	if (const uint updateCount = pState->TryUpdateResources(stage, shader.GetConstantBuffers()); updateCount > 0)
		SetConstantBuffers(pCtx.Get(), stage, 0, updateCount, ss.cbuffers.GetData());

	// Bind Samplers
	if (const uint updateCount = pState->TryUpdateResources(stage, resSrc.GetSamplers(), shader.GetSampMap()); updateCount > 0)
		SetSamplers(pCtx.Get(), stage, 0, updateCount, ss.samplers.GetData());

	// Bind Shader Resource Views (SRVs)
	if (const uint updateCount = pState->TryUpdateResources(stage, resSrc.GetSRVs(), shader.GetSRVMap()); updateCount > 0)
	{ 
		HandleConflicts();
		SetShaderResources(pCtx.Get(), stage, 0, updateCount, ss.srvs.GetData());
	}

	// CS specific handling
	if (stage == ShadeStages::Compute)
		BindResources(static_cast<const ComputeShaderVariant&>(shader), resSrc);
	// VS specific handling
	else if (stage == ShadeStages::Vertex)
		BindResources(static_cast<const VertexShaderVariant&>(shader), resSrc);
}

void CtxBase::UnbindShader(const ShaderVariantBase& shader)
{
	const ShadeStages stage = shader.GetStage();
	ContextState::StageState& ss = pState->GetStage(stage);

	if (ss.pShader == &shader)
	{
		SetShader(pCtx.Get(), stage, nullptr, nullptr, 0);
		ss.pShader = nullptr;
	}
}

void CtxBase::UnbindStage(ShadeStages stage) 
{
	const ShaderVariantBase* pShader = pState->GetStage(stage).pShader;

	if (pShader != nullptr)
		UnbindShader(*pShader);
}

void CtxBase::Dispatch(const ComputeShaderVariant& cs, ivec3 groups, const ResourceSet& res) 
{
	BindShader(cs, res);
	pCtx->Dispatch(groups.x, groups.y, groups.z);
}

void CtxBase::Draw(Mesh& mesh, Material& mat)
{
	Span meshSpan(&mesh);
	Draw(meshSpan, mat);
}

void CtxBase::Draw(IDynamicArray<Mesh>& meshes, Material& mat)
{
	for (Mesh& mesh : meshes)
	{
		mesh.Setup(*this);

		for (uint pass = 0; pass < mat.GetPassCount(); pass++)
		{
			mat.Setup(*this, pass);
			pCtx->DrawIndexed(mesh.GetIndexCount(), 0, 0);
			mat.Reset(*this, pass);
		}
	}
}

static void WriteBufferSubresource(ID3D11DeviceContext& ctx, BufferBase& dst, const IDynamicArray<byte>& src)
{
	ctx.UpdateSubresource(dst.Get(), 0, nullptr, src.GetData(), 0, 0);
}

static void WriteBufferMapUnmap(ID3D11DeviceContext& ctx, BufferBase& dst, const IDynamicArray<byte>& src)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	D3D_CHECK_HR(ctx.Map(
		dst.Get(),
		0u,
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	memcpy(msr.pData, src.GetData(), dst.GetSize());
	ctx.Unmap(dst.Get(), 0u);
}

void CtxBase::SetBufferData(BufferBase& dst, const IDynamicArray<byte>& src)
{
	if (dst.GetSize() > 0)
	{
		D3D_ASSERT_MSG(dst.GetUsage() != ResourceUsages::Immutable, "Cannot update Buffers without write access.");
		D3D_ASSERT_MSG(src.GetLength() == dst.GetSize(), "Buffer data source and destination size must be the same.");

		if (dst.GetUsage() == ResourceUsages::Dynamic)
			WriteBufferMapUnmap(*pCtx.Get(), dst, src);
		else
			WriteBufferSubresource(*pCtx.Get(), dst, src);
	}
}

static void WriteTexSubresource(ID3D11DeviceContext& ctx, ID3D11Resource& dst, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim, uivec2 dstOffset)
{
	D3D11_BOX dstBox;
	dstBox.left = dstOffset.x;
	dstBox.right = dstOffset.x + srcDim.x;
	dstBox.top = dstOffset.y;
	dstBox.bottom = dstOffset.y + srcDim.y;
	dstBox.front = 0;
	dstBox.back = 1;

	ctx.UpdateSubresource(
		&dst,
		0,
		&dstBox,
		src.GetData(),
		(UINT)(pixStride * srcDim.x),
		(UINT)(pixStride * srcDim.x * srcDim.y)
	);
}

static void WriteTexMapUnmap(ID3D11DeviceContext& ctx, ID3D11Resource& dst, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim, uivec2 dstOffset)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	D3D_CHECK_HR(ctx.Map(
		&dst,
		0u, // Subresource 0
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	const size_t srcRowPitch = (size_t)srcDim.x * pixStride;
	const size_t dstRowPitch = msr.RowPitch;
	byte* pDst = static_cast<byte*>(msr.pData);
	const size_t dstRowOffset = dstOffset.x * pixStride;

	// Copy source image into texture row-by-row to account for variability in buffer alignment
	// and horizontal offset specified in dstOffset
	for (uint row = 0; row < srcDim.y; row++)
	{
		memcpy(
			pDst + ((row + dstOffset.y) * dstRowPitch) + dstRowOffset,
			&src[row * srcRowPitch],
			srcRowPitch
		);
	}

	ctx.Unmap(&dst, 0u);
}

void CtxBase::SetTextureData(ITexture2DBase& dst, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim, uivec2 dstOffset)
{
	D3D_CHECK_MSG(dst.GetUsage() != ResourceUsages::Immutable, "Cannot update Textures without write access.");
	const uivec2 dstSize = dst.GetSize();
	const uivec2 dstMax = srcDim + dstOffset;
	D3D_ASSERT_MSG(dstMax.x <= dstSize.x && dstMax.y <= dstSize.y, "Texture write destination out of bounds");

	if (dst.GetUsage() == ResourceUsages::Dynamic)
		WriteTexMapUnmap(*pCtx.Get(), *dst.GetResource(), src, pixStride, srcDim, dstOffset);
	else
		WriteTexSubresource(*pCtx.Get(), *dst.GetResource(), src, pixStride, srcDim, dstOffset);
}

static void ValidateResourceBounds(
	IColorBuffer2D& src,
	IColorBuffer2D& dst,
	ivec4& srcBox,
	ivec4& dstBox,
	bool canRescale = false)
{
	srcBox = glm::max(srcBox, ivec4(0));
	dstBox = glm::max(dstBox, ivec4(0));

	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{
		if (srcBox == ivec4(0))
			srcBox = ivec4(src.GetSize(), ivec2(0));

		if (dstBox == ivec4(0))
			dstBox = ivec4(dst.GetSize(), ivec2(0));

		uivec2
			srcSize(srcBox.x, srcBox.y),
			dstSize(dstBox.x, dstBox.y),
			srcOffset(srcBox.z, srcBox.w),
			dstOffset(dstBox.z, dstBox.w),
			srcMax = glm::min(src.GetSize(), srcOffset + srcSize) - srcSize,
			dstMax = glm::min(dst.GetSize(), dstOffset + dstSize) - dstSize;

		D3D_CHECK_MSG(srcMax == srcOffset,
			"[Blit] Source subregion exceeds the bounds of the source buffer.");

		D3D_CHECK_MSG(dstMax == dstOffset,
			"[Blit] Destination subregion exceeds the bounds of the destination buffer.");

		if (!canRescale)
		{
			srcMax = srcOffset + srcSize;
			dstMax = dstOffset + dstSize;

			D3D_CHECK_MSG(srcMax.x <= dstMax.x && srcMax.y <= dstMax.y,
				"[Blit] Source subregion exceeds the bounds of the destination buffer.");
		}
	}
}

static bool CanDirectCopy(ITexture2DBase& src, ITexture2DBase& dst, const ivec4& srcBox, const ivec4& dstBox)
{
	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{
		return
			dst.GetUsage() != ResourceUsages::Immutable &&
			srcBox.x == dstBox.x && srcBox.y == dstBox.y &&
			src.GetFormat() == dst.GetFormat();
	}
	else
	{
		return
			dst.GetUsage() != ResourceUsages::Immutable &&
			src.GetSize() == dst.GetSize() &&
			src.GetFormat() == dst.GetFormat();
	}
}

static void CopySubresource(ID3D11DeviceContext* pCtx, ITexture2DBase& src, ITexture2DBase& dst, ivec4& srcBox, ivec4& dstBox)
{
	if (srcBox != ivec4(0) || dstBox != ivec4(0))
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox);

		D3D11_BOX srcBB =
		{
			.left = (uint)std::max(srcBox.z, 0),
			.top = (uint)std::max(srcBox.w, 0),
			.front = 0u,
			.right = (uint)std::max(srcBox.z + srcBox.x, 0),
			.bottom = (uint)std::max(srcBox.w + srcBox.y, 0),
			.back = 1u
		};

		pCtx->CopySubresourceRegion(
			dst.GetResource(), 0u,
			(uint)dstBox.z, (uint)dstBox.w, 0u, // Dst start bounds
			src.GetResource(), 0u,
			&srcBB
		);
	}
	else
		pCtx->CopyResource(dst.GetResource(), src.GetResource());
}

void CtxBase::ReturnMappedBufferHandle(MappedBufferHandle&& handle)
{
	D3D_ASSERT_MSG(handle.GetIsValid(), "Attempted to release an invalid buffer handle.");
	pCtx->Unmap(handle.GetParent().GetResource(), 0u);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(IResizeableTexture2D& src, IRWTexture2D& dst)
{
	Blit(static_cast<ITexture2D&>(src), dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox)
{
	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);

	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (CanDirectCopy(src, dst, srcBox, dstBox))
	{
		CopySubresource(pCtx.Get(), src, dst, srcBox, dstBox);
		return;
	}

	if (srcBox.x == dstBox.x && srcBox.y == dstBox.y)
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox);

		Renderer& renderer = GetRenderer();
		ComputeInstance& cs = renderer.GetDefaultCompute("TexCopy2D");

		cs.SetTexture("SrcTex", src);
		cs.SetRWTexture("DstTex", dst);
		cs.SetConstant("SrcOffset", ivec2(srcBox.z, srcBox.w));
		cs.SetConstant("DstOffset", ivec2(dstBox.z, dstBox.w));
		cs.Dispatch(*this, { dstBox.x, dstBox.y });
	}
	else
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox, true);

		Renderer& renderer = GetRenderer();
		ComputeInstance& cs = renderer.GetDefaultCompute("TexCopyScaledSamp2D");

		const ivec2 srcRenderSize = vec2(srcBox.x, srcBox.y);
		const vec2 scale = vec2(srcRenderSize) / vec2(src.GetSize()),
			offset = vec2(srcBox.z, srcBox.w) * vec2(src.GetTexelSize());

		cs.SetTexture("SrcTex", src);
		cs.SetRWTexture("DstTex", dst);
		cs.SetSampler("Samp", renderer.GetDefaultSampler("LinearBorder"));
		cs.SetConstant("Scale", scale);
		cs.SetConstant("Offset", offset);
		cs.SetConstant("SrcOffset", ivec2(srcBox.z, srcBox.w));
		cs.SetConstant("DstOffset", ivec2(dstBox.z, dstBox.w));
		cs.SetConstant("DstTexelSize", dst.GetRenderTexelSize());
		cs.Dispatch(*this, { dstBox.x, dstBox.y });
	}
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox)
{
	Blit(
		src,
		dst,
		srcBox,
		ivec4(dst.GetRenderSize(), dst.GetRenderOffset())
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox)
{
	Blit(
		src,
		dst,
		ivec4(src.GetRenderSize(), src.GetRenderOffset()),
		dstBox
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(IResizeableTexture2D& src, IResizeableTexture2D& dst)
{
	Blit(
		src,
		dst,
		ivec4(src.GetRenderSize(), src.GetRenderOffset()),
		ivec4(dst.GetRenderSize(), dst.GetRenderOffset())
	);
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox, ivec4 dstBox)
{
	D3D_CHECK_MSG(CanDirectCopy(src, dst, srcBox, dstBox), "Failed to copy texture. Destination incompatible with source.");
	ValidateResourceBounds(src, dst, srcBox, dstBox);
	CopySubresource(pCtx.Get(), src, dst, srcBox, dstBox);
}

/// <summary>
/// Copies the contents of a texture to a render target
/// </summary>
void CtxBase::Blit(IResizeableTexture2D& src, IRenderTarget& dst)
{
	Blit(src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(ITexture2D& src, IRenderTarget& dst, ivec4 srcBox)
{
	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);
	else
		ValidateResourceBounds(src, dst, srcBox, dstBox, true);

	Renderer& renderer = GetRenderer();
	Mesh& quad = renderer.GetDefaultMesh("FSQuad");
	Material& quadFX = renderer.GetDefaultMaterial("PosTextured2D");
	Viewport vp =
	{
		.offset = dst.GetRenderOffset(),
		.size = dst.GetRenderSize(),
		.zDepth = vec2(0)
	};
	bool wasRtvSet = false;
	bool wasVpSet = false;

	// Manually set render target if it isn't already set
	if (dst.GetRTV() != pState->rtvs[0])
	{
		pCtx->OMSetRenderTargets(1, dst.GetAddressRTV(), nullptr);
		wasRtvSet = true;
	}
	// Manually set VP if it isn't set
	if (vp != pState->viewports[0])
	{
		pCtx->RSSetViewports(1, (D3D11_VIEWPORT*)&vp);
		wasVpSet = true;
	}

	const uivec2 srcRenderSize = vec2(srcBox.x, srcBox.y);
	const vec2 scale = vec2(srcRenderSize) / vec2(src.GetSize()),
		offset = vec2(srcBox.z, srcBox.w) * vec2(src.GetTexelSize());

	if (srcRenderSize == dst.GetRenderSize())
		quadFX.SetSampler("samp", renderer.GetDefaultSampler("PointBorder"));
	else
		quadFX.SetSampler("samp", renderer.GetDefaultSampler("LinearClamp"));

	quadFX.SetConstant("Scale", scale);
	quadFX.SetConstant("Offset", offset);
	quadFX.SetTexture("tex", src);

	Draw(quad, quadFX);

	// Manually restore previous state if changed
	if (wasRtvSet)
		pCtx->OMSetRenderTargets(pState->rtCount, pState->rtvs.GetData(), pState->GetDepthStencilView());
	if (wasVpSet)
		pCtx->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
}

void CtxBase::ClearDepthStencil(IDepthStencil& ds, DSClearFlags clearFlags, float depthClear, UINT8 stencilClear)
{
	pCtx->ClearDepthStencilView(ds.GetDSV(), (UINT)clearFlags, depthClear, stencilClear);
}

void CtxBase::ResetShaders() 
{
	for (int i = 0; i < g_ShadeStageCount; i++)
		UnbindStage((ShadeStages)i);
}

void CtxBase::Reset()
{
	ResetShaders();
	UnbindVertexBuffers();
	SetPrimitiveTopology(PrimTopology::UNDEFINED);
	UnbindRenderTargets();
	UnbindViewports();
}


bool CtxBase::GetIsImmediate() const { return isImmediate; }

const uint CtxBase::GetViewportCount() const { return (uint)pState->vpCount; }

const Viewport& CtxBase::GetViewport(uint index) const { return pState->viewports[index]; }

const Span<Viewport> CtxBase::GetViewports() const { return Span(pState->viewports.GetData(), pState->vpCount); }

void CtxBase::BindViewport(sint index, const vec2& size, const vec2& offset, const vec2& depth)
{
	Viewport vp = { offset, size, depth };
	BindViewports(Span(&vp), index);
}

void CtxBase::BindViewport(sint index, const Viewport& vp)
{
	BindViewports(Span((Viewport*)&vp), index);
}

void CtxBase::UnbindViewports(sint index, uint count)
{
	if (pState->vpCount == 0 || count == 0)
		return;

	if (count == -1)
	{
		index = 0;
		count = pState->vpCount;
	}

	D3D_ASSERT_MSG((index + count) <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		"Viewport range specified out of range.");

	SetArrNull(pState->viewports, index, count);
	pCtx->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
	pState->vpCount = index;
}

void CtxBase::BindViewports(const IDynamicArray<Viewport>& viewports, sint offset)
{
	D3D_ASSERT_MSG((offset + viewports.GetLength()) <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		"Number of viewports supplied exceeds limit.");

	const uint newCount = (uint)viewports.GetLength();
	Span dstSpan(&pState->viewports[offset], newCount);

	if (dstSpan != viewports)
	{
		memcpy(dstSpan.GetData(), viewports.GetData(), GetArrSize(viewports));
		pState->vpCount = std::max(pState->vpCount, (uint)(offset + newCount));
		pCtx->RSSetViewports(pState->vpCount, (D3D11_VIEWPORT*)pState->viewports.GetData());
	}
}
