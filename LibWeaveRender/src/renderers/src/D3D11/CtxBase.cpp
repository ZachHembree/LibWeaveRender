#include "pch.hpp"
#include "WeaveEffects/ShaderData.hpp"
#include "D3D11/InternalD3D11.hpp"
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

DEF_DEST_MOVE(CtxBase);

CtxBase::CtxBase() = default;

CtxBase::CtxBase(Device& dev, UniqueComPtr<ID3D11DeviceContext1>&& pCtx) :
	DeviceChild(dev),
	pCtx(std::move(pCtx)),
	pState(new ContextState()),
	isImmediate(this->pCtx->GetType() == D3D11_DEVICE_CONTEXT_IMMEDIATE)
{
	pState->Init();
}

void CtxBase::BindDepthStencilBuffer(IDepthStencil& depthStencil) 
{
	if (pState->TryUpdateDepthStencil(&depthStencil))
	{
		HandleConflicts();
		pCtx->OMSetDepthStencilState(pState->GetDepthStencilState(), 1u);
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
	}
}

void CtxBase::UnbindDepthStencilBuffer() 
{
	if (pState->TryUpdateDepthStencil(nullptr))
	{
		pCtx->OMSetDepthStencilState(pState->GetDepthStencilState(), 1u);
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
	}
}

uint CtxBase::GetRenderTargetCount() const { return (uint)pState->GetRenderTargets().GetLength(); }

void CtxBase::BindRenderTarget(IRenderTarget& rt, IDepthStencil& ds, sint slot)
{
	IRenderTarget* pRT = &rt;
	Span rtSpan(&pRT);
	BindRenderTargets(rtSpan, &ds, slot);
}

void CtxBase::BindRenderTarget(IRenderTarget& rt, sint slot)
{
	IRenderTarget* pRT = &rt;
	Span rtSpan(&pRT);
	BindRenderTargets(rtSpan, slot);
}

void CtxBase::BindRenderTargets(IDynamicArray<IRenderTarget*>& rts, sint startSlot)
{
	D3D_ASSERT_MSG(rts.GetLength() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"Number of render targets supplied exceeds limit.");

	if (pState->TryUpdateRenderTargets(rts, startSlot))
	{
		HandleConflicts();
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
	}

	if (pState->TryUpdateViewports(rts, startSlot))
	{
		const Span<Viewport> viewports = pState->GetViewports();
		pCtx->RSSetViewports((uint)viewports.GetLength(), (D3D11_VIEWPORT*)&viewports[0]);
	}
}

void CtxBase::BindRenderTargets(IDynamicArray<IRenderTarget*>& rts, IDepthStencil* pDS, sint startSlot)
{
	D3D_ASSERT_MSG(rts.GetLength() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"Number of render targets supplied exceeds limit.");

	const bool wasDsChanged = pState->TryUpdateDepthStencil(pDS);

	if (wasDsChanged)
		pCtx->OMSetDepthStencilState(pState->GetDepthStencilState(), 1);

	const bool wasRtvChanged = pState->TryUpdateRenderTargets(rts, startSlot);

	if (wasDsChanged || wasRtvChanged)
	{
		HandleConflicts();
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
	}
		
	if (pState->TryUpdateViewports(rts, startSlot))
	{
		const Span<Viewport> viewports = pState->GetViewports();
		pCtx->RSSetViewports((uint)viewports.GetLength(), (D3D11_VIEWPORT*)&viewports[0]);
	}
}

void CtxBase::UnbindRenderTarget(sint slot)
{
	UnbindRenderTargets(slot, 1);
}

void CtxBase::UnbindRenderTargets(sint startSlot, uint count)
{
	if (count == (uint)-1)
	{
		startSlot = 0;
		count = (uint)pState->GetRenderTargets().GetLength();
	}

	Span<IRenderTarget*> nullRTs;
	
	if (count > 0)
		ALLOCA_SPAN_SET_NULL(nullRTs, count, IRenderTarget*);

	if (pState->TryUpdateRenderTargets(nullRTs, startSlot))
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
}

void CtxBase::SetPrimitiveTopology(PrimTopology topology)
{
	if (pState->TryUpdateTopology(topology))
		pCtx->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)topology);
}

void CtxBase::BindVertexBuffer(VertexBuffer& vertBuffer, sint slot)
{
	Span vbSpan(&vertBuffer);
	BindVertexBuffers(vbSpan, slot);
}

void CtxBase::BindVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, sint startSlot)
{
	if (const uint extent = pState->TryUpdateVertexBuffers(vertBuffers, startSlot); (sint)extent > startSlot)
	{
		const uint count = (extent - startSlot);
		pCtx->IASetVertexBuffers(
			startSlot, count,
			&pState->GetVertexBuffers()[startSlot],
			&pState->GetVertStrides()[startSlot],
			&pState->GetVertOffsets()[startSlot]
		);
	}
}

void CtxBase::UnbindVertexBuffer(sint slot)
{
	UnbindVertexBuffers(slot, 1);
}

void CtxBase::UnbindVertexBuffers(sint startSlot, uint count)
{
	if (const uint extent = pState->TryResetVertexBuffers(startSlot, count); extent > 0)
	{
		if (pState->GetVertexBuffers().GetLength() > 0)
		{
			pCtx->IASetVertexBuffers(
				startSlot, count,
				&pState->GetVertexBuffers()[startSlot],
				&pState->GetVertStrides()[startSlot],
				&pState->GetVertOffsets()[startSlot]
			);
		}
		else
		{
			pCtx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		}
	}
}

void CtxBase::BindIndexBuffer(IndexBuffer& indexBuffer, uint byteOffset)
{
	if (pState->TryUpdateIndexBuffer(indexBuffer.GetBuffer()))
	{
		pCtx->IASetIndexBuffer(pState->GetIndexBuffer(), (DXGI_FORMAT)indexBuffer.GetFormat(), byteOffset);
	}
}

void CtxBase::UnbindIndexBuffer()
{
	if (pState->TryUpdateIndexBuffer(nullptr))
	{
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
				SetShaderResources(pCtx.Get(), stage, 0, ss.GetSRVs(0, conflicts.srvsExtent[i]));
		}

		if (conflicts.uavsExtent > 0)
			CSSetUnorderedAccessViews(pCtx.Get(), 0, pState->CSGetUAVs(0, conflicts.uavsExtent), nullptr);

		if (conflicts.rtvExtent > 0)
			OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());
	}
}

void CtxBase::BindResources(const ComputeShaderVariant& shader, const ResourceSet& resSrc)
{
	if (const auto& newUAVs = pState->TryUpdateResources(resSrc.GetUAVs(), shader.GetUAVMap()); !newUAVs.IsEmpty())
	{
		HandleConflicts();
		CSSetUnorderedAccessViews(pCtx.Get(), 0, newUAVs, nullptr);
	}
}

void CtxBase::BindResources(const VertexShaderVariant& shader, const ResourceSet& resSrc)
{
	ID3D11InputLayout* pLayout = shader.GetInputLayout().Get();
	
	if (pState->TryUpdateInputLayout(pLayout))
	{
		pCtx->IASetInputLayout(pState->GetInputLayout());
	}
}

void CtxBase::BindShader(const ShaderVariantBase& shader, const ResourceSet& resSrc)
{
	const ShadeStages stage = shader.GetStage();

	// Bind shader
	if (pState->TrySetShader(stage, &shader))
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
	if (const auto& newCBufs = pState->TryUpdateResources(stage, shader.GetConstantBuffers()); !newCBufs.IsEmpty())
		SetConstantBuffers(pCtx.Get(), stage, 0, newCBufs);

	// Bind Samplers
	if (const auto& newSamps = pState->TryUpdateResources(stage, resSrc.GetSamplers(), shader.GetSampMap()); !newSamps.IsEmpty())
		SetSamplers(pCtx.Get(), stage, 0, newSamps);

	// Bind Shader Resource Views (SRVs)
	if (const auto& newSRVs = pState->TryUpdateResources(stage, resSrc.GetSRVs(), shader.GetSRVMap()); !newSRVs.IsEmpty())
	{ 
		HandleConflicts();
		SetShaderResources(pCtx.Get(), stage, 0, newSRVs);
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
	const ContextState::StageState& ss = pState->GetStage(stage);

	if (ss.pShader == &shader && pState->TrySetShader(stage, nullptr))
		SetShader(pCtx.Get(), stage, nullptr, nullptr, 0);
}

void CtxBase::UnbindStage(ShadeStages stage) 
{
	if (pState->TrySetShader(stage, nullptr))
		SetShader(pCtx.Get(), stage, nullptr, nullptr, 0);
}

void CtxBase::SetActiveStages(ActiveShaderMask activeShaders)
{
	for (uint i = 0; i < activeShaders.size(); i++)
	{		
		if (!activeShaders[i])
			UnbindStage((ShadeStages)i);
	}
}

void CtxBase::Dispatch(const ComputeShaderVariant& cs, uivec3 groups, const ResourceSet& res) 
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
			pState->IncrementDraw();
			mat.Setup(*this, pass);
			pCtx->DrawIndexed(mesh.GetIndexCount(), 0, 0);
		}
	}
}

static void WriteBufferSubresource(ID3D11DeviceContext& ctx, BufferBase& dst, const IDynamicArray<byte>& src)
{
	ctx.UpdateSubresource(dst.GetBuffer(), 0, nullptr, src.GetData(), 0, 0);
}

static void WriteBufferMapUnmap(ID3D11DeviceContext& ctx, BufferBase& dst, const IDynamicArray<byte>& src)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	D3D_CHECK_HR(ctx.Map(
		dst.GetBuffer(),
		0u,
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	memcpy(msr.pData, src.GetData(), dst.GetSize());
	ctx.Unmap(dst.GetBuffer(), 0u);
}

void CtxBase::SetBufferData(BufferBase& dst, const IDynamicArray<byte>& src)
{
	if (dst.GetSize() > 0)
	{
		D3D_ASSERT_MSG(dst.GetUsage() != ResourceUsages::Immutable, "Cannot update Buffers without write access.");

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
	const IColorBuffer2D& src,
	const IColorBuffer2D& dst,
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

static bool CanDirectCopy(const ITexture2DBase& src, const ITexture2DBase& dst, const ivec4& srcBox, const ivec4& dstBox)
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

static void CopySubresource(ID3D11DeviceContext* pCtx, const ITexture2DBase& src, ITexture2DBase& dst, ivec4& srcBox, ivec4& dstBox)
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
void CtxBase::Blit(const IResizeableTexture2D& src, IRWTexture2D& dst)
{
	Blit(static_cast<const ITexture2D&>(src), dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(const ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox)
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
		cs.DispatchThreads(*this, { dstBox.x, dstBox.y });
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
		cs.DispatchThreads(*this, { dstBox.x, dstBox.y });
	}
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(const ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox)
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
void CtxBase::Blit(const IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox)
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
void CtxBase::Blit(const IResizeableTexture2D& src, IResizeableTexture2D& dst)
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
void CtxBase::Blit(const ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox, ivec4 dstBox)
{
	D3D_CHECK_MSG(CanDirectCopy(src, dst, srcBox, dstBox), "Failed to copy texture. Destination incompatible with source.");
	ValidateResourceBounds(src, dst, srcBox, dstBox);
	CopySubresource(pCtx.Get(), src, dst, srcBox, dstBox);
}

/// <summary>
/// Copies the contents of a texture to a render target
/// </summary>
void CtxBase::Blit(const IResizeableTexture2D& src, IRenderTarget& dst)
{
	Blit(src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void CtxBase::Blit(const ITexture2D& src, IRenderTarget& dst, ivec4 srcBox)
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
	if (dst != pState->GetRenderTargets()[0])
	{
		pCtx->OMSetRenderTargets(1, dst.GetAddressRTV(), nullptr);
		wasRtvSet = true;
	}
	// Manually set VP if it isn't set
	if (vp != pState->GetViewports()[0])
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
		OMSetRenderTargets(pCtx.Get(), pState->GetRenderTargets(), pState->GetDepthStencilView());

	if (wasVpSet)
	{
		const Span<Viewport> viewports = pState->GetViewports();
		pCtx->RSSetViewports((uint)viewports.GetLength(), (D3D11_VIEWPORT*)&viewports[0]);
	}
}

void CtxBase::ClearDepthStencil(IDepthStencil& ds, DSClearFlags clearFlags, float depthClear, byte stencilClear)
{
	pCtx->ClearDepthStencilView(ds.GetDSV(), clearFlags.value, depthClear, stencilClear);
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
}

bool CtxBase::GetIsImmediate() const { return isImmediate; }

const uint CtxBase::GetViewportCount() const { return (uint)pState->GetViewports().GetLength(); }

const Viewport& CtxBase::GetViewport(uint index) const { return pState->GetViewports()[index]; }

const Span<Viewport> CtxBase::GetViewports() const { return pState->GetViewports(); }
