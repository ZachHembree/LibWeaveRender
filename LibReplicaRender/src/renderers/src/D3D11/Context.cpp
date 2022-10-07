#include "ReplicaD3D11.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Context::Context(Device& dev, ComPtr<ID3D11DeviceContext>& pContext) :
	DeviceChild(dev),
	pContext(pContext),
	currentVS(nullptr),
	currentPS(nullptr),
	currentCS(nullptr),
	currentVPs(D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE),
	currentRTVs(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr),
	currentDSV(nullptr),
	currentDSS(nullptr),
	vpCount(0),
	rtvCount(0)
{ 
	IASetPrimitiveTopology(PrimTopology::TRIANGLELIST);
}

Context::Context() : 
	DeviceChild(),
	currentVS(nullptr),
	currentPS(nullptr),
	currentCS(nullptr),
	currentDSV(nullptr),
	currentDSS(nullptr),
	rtvCount(0)
{ }

/// <summary>
/// Returns reference to renderer using this context
/// </summary>
Renderer& Context::GetRenderer()
{
	return GetDevice().GetRenderer();
}

void Context::SetVS(VertexShader* vs)
{
	if (vs != currentVS || currentVS == nullptr || vs == nullptr)
	{
		if ((vs != currentVS) && currentVS != nullptr)
			currentVS->Unbind();

		ID3D11VertexShader* pVs = vs != nullptr ? vs->Get() : nullptr;
		Get().VSSetShader(pVs, nullptr, 0);
		currentVS = vs;
	}
}

void Context::SetPS(PixelShader* ps)
{
	if (ps != currentPS || currentPS == nullptr || ps == nullptr)
	{ 
		if ((ps != currentPS) && currentPS != nullptr)
			currentPS->Unbind();

		ID3D11PixelShader* pPs = ps != nullptr ? ps->Get() : nullptr;
		Get().PSSetShader(pPs, nullptr, 0);
		currentPS = ps;
	}
}

void Context::SetCS(ComputeShader* cs)
{
	if (cs != currentCS || currentCS == nullptr || cs == nullptr)
	{
		if ((cs != currentCS) && currentCS != nullptr)
			currentCS->Unbind();

		ID3D11ComputeShader* pCS = cs != nullptr ? cs->Get() : nullptr;
		Get().CSSetShader(pCS, nullptr, 0);
		currentCS = cs;
	}
}

bool Context::GetIsVsBound(VertexShader* vs) const
{
	return currentVS != nullptr && vs == currentVS;
}

bool Context::GetIsPsBound(PixelShader* ps) const
{
	return currentPS != nullptr && ps == currentPS;
}

bool Context::GetIsCsBound(ComputeShader* cs) const
{
	return currentCS != nullptr && cs == currentCS;
}

void Context::Reset()
{
	SetVS(nullptr);
	SetPS(nullptr);
	SetCS(nullptr);
	pContext->OMSetRenderTargets(0, nullptr, nullptr);
	pContext->RSSetViewports(0, nullptr);
	pContext->OMSetDepthStencilState(nullptr, 0);

	currentDepthRange = vec2(0);
	currentDSV = nullptr;
	currentDSS = nullptr;

	memset(currentVPs.GetPtr(), 0, currentVPs.GetSize());
	vpCount = 0;

	memset(currentRTVs.GetPtr(), 0, currentRTVs.GetSize());
	rtvCount = 0;

	vpSpan = Span<Viewport>();
	rtvSpan = Span<ID3D11RenderTargetView*>();
}

/// <summary>
/// Returns pointer to context interface
/// </summary>
ID3D11DeviceContext& Context::Get() const { return *pContext.Get(); }

/// <summary>
/// Returns reference to context interface
/// </summary>
ID3D11DeviceContext* Context::operator->() const { return pContext.Get(); }

/// <summary>
/// Returns the number of viewports currently bound
/// </summary>
const int  Context::GetViewportCount() const
{
	return vpCount;
}

/// <summary>
/// Returns the viewport bound to the given index.
/// </summary>
const Viewport& Context::GetViewport(int index) const
{
	return vpSpan[index];
}

/// <summary>
/// Returns an array of the viewports currently bound
/// </summary>
const IDynamicArray<Viewport>& Context::GetViewports() const
{
	return vpSpan;
}

/// <summary>
/// Binds the given viewport to the rasterizer stage
/// </summary>
void Context::SetViewport(int index, const vec2& size, const vec2& offset, const vec2& depth)
{
	Viewport vp = { offset, size, depth };
	SetViewports(Span(&vp), index);
}

/// <summary>
/// Binds the given viewport to the rasterizer stage
/// </summary>
void Context::SetViewport(int index, const Viewport& vp)
{
	SetViewports(Span((Viewport*)&vp), index);
}

/// <summary>
/// Binds the given collection of viewports to the rasterizer stage
/// </summary>
void Context::SetViewports(const IDynamicArray<Viewport>& viewports, int offset)
{
	GFX_ASSERT((offset + viewports.GetLength()) <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		"Number of viewports supplied exceeds limit.")

	memcpy(currentVPs.GetPtr() + glm::max(offset, 0), viewports.GetPtr(), viewports.GetSize());

	vpCount = std::max(vpCount, (uint)(offset + viewports.GetLength()));
	pContext->RSSetViewports(vpCount, (D3D11_VIEWPORT*)currentVPs.GetPtr());

	vpSpan = Span(currentVPs.GetPtr(), vpCount);
}

/// <summary>
/// Binds the given buffer as the depth stencil buffer doesn't overwrite render targets. Set to nullptr
/// to unbind.
/// </summary>
void Context::SetDepthStencilBuffer(IDepthStencil& depthStencil)
{
	if (currentDSS != depthStencil.GetState())
	{
		currentDSS = depthStencil.GetState();
		pContext->OMSetDepthStencilState(currentDSS, 1);
		currentDepthRange = depthStencil.GetRange();
	}

	currentDSV = depthStencil.GetDSV();
	pContext->OMSetRenderTargets(rtvCount, currentRTVs.GetPtr(), currentDSV);
}

/// <summary>
/// Returns the number of render targets currently bound
/// </summary>
int Context::GetRenderTargetCount() const
{
	return rtvCount;
}

/// <summary>
/// Returns the render target view bound to the given index.
/// </summary>
ID3D11RenderTargetView* Context::GetRenderTarget(int index) const
{
	return rtvSpan[index];
}

/// <summary>
/// Returns an array of the render target views currently bound
/// </summary>
const IDynamicArray<ID3D11RenderTargetView*>& Context::GetRenderTargets() const
{
	return rtvSpan;
}

/// <summary>
/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
/// </summary>
void Context::SetRenderTarget(IRenderTarget& rt, IDepthStencil& ds)
{
	SetRenderTarget(rt, &ds);
}

/// <summary>
/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
/// </summary>
void Context::SetRenderTargets(IDynamicArray<IRenderTarget>& rtvs, IDepthStencil& ds)
{
	SetRenderTargets(rtvs, &ds);
}

/// <summary>
/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
/// </summary>
void Context::SetRenderTarget(IRenderTarget& rtv, IDepthStencil* depthStencil)
{
	Span rtvSpan(&rtv);
	SetRenderTargets(rtvSpan, depthStencil);
}

/// <summary>
/// Binds the given buffers as render targets. Doesn't unbind previously set depth-stencil buffers.
/// </summary>
void Context::SetRenderTargets(IDynamicArray<IRenderTarget>& rtvs, IDepthStencil* depthStencil)
{
	GFX_ASSERT(rtvs.GetLength() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"Number of render targets supplied exceeds limit.")

	if (depthStencil != nullptr)
	{
		if (currentDSS != depthStencil->GetState())
		{
			currentDSS = depthStencil->GetState();
			pContext->OMSetDepthStencilState(currentDSS, 1);
			currentDepthRange = depthStencil->GetRange();
		}

		currentDSV = depthStencil->GetDSV();
	}
	else
	{
		currentDSV = nullptr;
		currentDSS = nullptr;
	}

	rtvCount = (uint)rtvs.GetLength();

	for (int i = 0; i < (int)rtvCount; i++)
		currentRTVs[i] = rtvs[i].GetRTV();

	pContext->OMSetRenderTargets(rtvCount, currentRTVs.GetPtr(), currentDSV);
	rtvSpan = Span(currentRTVs.GetPtr(), rtvCount);

	// Update viewports for render textures
	Viewport viewports[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]({});

	for (int i = 0; i < (int)rtvCount; i++)
	{
		viewports[i] = 
		{ 
			rtvs[i].GetRenderOffset(), 
			rtvs[i].GetRenderSize(), 
			currentDepthRange 
		};
	}

	SetViewports(Span((Viewport*)&viewports, rtvCount));
}

/// <summary>
/// Binds a vertex buffer to the given slot
/// </summary>
void Context::IASetVertexBuffer(VertexBuffer& vertBuffer, int slot)
{
	UINT offset = 0, stride = vertBuffer.GetStride();
	pContext->IASetVertexBuffers(slot, 1, vertBuffer.GetAddressOf(), &stride, &offset);
}

/// <summary>
/// Determines how vertices are interpreted by the input assembler
/// </summary>
void Context::IASetPrimitiveTopology(PrimTopology topology)
{
	pContext->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)topology);
}

/// <summary>
/// Binds an array of buffers starting at the given slot
/// </summary>
void Context::IASetVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, int startSlot)
{
	for (int i = 0; i < vertBuffers.GetLength(); i++)
	{
		IASetVertexBuffer(vertBuffers[i], startSlot + i);
	}
}

void ValidateResourceBounds(
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

		 ivec2
			srcSize(srcBox.x, srcBox.y),
			dstSize(dstBox.x, dstBox.y),
			srcOffset(srcBox.z, srcBox.w),
			dstOffset(dstBox.z, dstBox.w),
			srcMax = glm::min(src.GetSize(), srcOffset + srcSize) - srcSize,
			dstMax = glm::min(dst.GetSize(), dstOffset + dstSize) - dstSize;

		GFX_ASSERT(srcMax == srcOffset,
			"[Blit] Source subregion exceeds the bounds of the source buffer.");

		GFX_ASSERT(dstMax == dstOffset,
			"[Blit] Destination subregion exceeds the bounds of the destination buffer.");

		if (!canRescale)
		{
			srcMax = srcOffset + srcSize;
			dstMax = dstOffset + dstSize;

			GFX_ASSERT(srcMax.x <= dstMax.x && srcMax.y <= dstMax.y,
				"[Blit] Source subregion exceeds the bounds of the destination buffer.");
		}
	}
}

bool CanDirectCopy(ITexture2DBase& src, ITexture2DBase& dst, const ivec4& srcBox, const ivec4& dstBox)
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

void CopySubresource(Context& ctx, ITexture2DBase& src, ITexture2DBase& dst, ivec4& srcBox, ivec4& dstBox)
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

		ctx->CopySubresourceRegion(
			dst.GetResource(), 0u,
			(uint)dstBox.z, (uint)dstBox.w, 0u, // Dst start bounds
			src.GetResource(), 0u, 
			&srcBB
		);
	}
	else
		ctx->CopyResource(dst.GetResource(), src.GetResource());
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(IResizeableTexture2D& src, IRWTexture2D& dst)
{
	Blit((ITexture2D&)src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox)
{
	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);

	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (CanDirectCopy(src, dst, srcBox, dstBox))
	{
		CopySubresource(*this, src, dst, srcBox, dstBox);
		return;
	}

	if (srcBox.x == dstBox.x && srcBox.y == dstBox.y)
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox);

		Renderer& renderer = GetRenderer();
		ComputeShader& cs = renderer.GetDefaultCompute("TexCopy2D");

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
		ComputeShader& cs = renderer.GetDefaultCompute("TexCopyScaledSamp2D");

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
void Context::Blit(ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox)
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
void Context::Blit(IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox)
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
void Context::Blit(IResizeableTexture2D& src, IResizeableTexture2D& dst)
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
void Context::Blit(ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox, ivec4 dstBox)
{
	if (CanDirectCopy(src, dst, srcBox, dstBox))
	{
		ValidateResourceBounds(src, dst, srcBox, dstBox);
		CopySubresource(*this, src, dst, srcBox, dstBox);
	}
	else
	{
		GFX_THROW("Failed to copy texture. Destination incompatible with source.");
	}
}

/// <summary>
/// Copies the contents of a texture to a render target
/// </summary>
void Context::Blit(IResizeableTexture2D& src, IRenderTarget& dst)
{
	Blit(src, dst, ivec4(src.GetRenderSize(), src.GetRenderOffset()));
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRenderTarget& dst, ivec4 srcBox)
{
	ivec4 dstBox(dst.GetRenderSize(), dst.GetRenderOffset());

	if (srcBox == ivec4(0))
		srcBox = ivec4(src.GetSize(), 0, 0);
	else
		ValidateResourceBounds(src, dst, srcBox, dstBox, true);

	Renderer& renderer = GetRenderer();
	Mesh& quad = renderer.GetDefaultMesh("FSQuad");
	Effect& quadFX = renderer.GetDefaultEffect("PosTextured2D");

	ID3D11RenderTargetView* const lastRTV = GetRenderTarget(0);
	const Viewport& lastVP = GetViewport(0);

	pContext->OMSetRenderTargets(1, dst.GetAddressRTV(), nullptr);
	SetViewport(0, dst.GetRenderSize(), dst.GetRenderOffset());

	const ivec2 srcRenderSize = vec2(srcBox.x, srcBox.y);
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

	pContext->OMSetRenderTargets(rtvCount, currentRTVs.GetPtr(), currentDSV);
	SetViewport(0, lastVP);
}

/// <summary>
/// Draw indexed, non-instanced triangle meshes using the given effect
/// </summary>
void Context::Draw(Mesh& mesh, Effect& effect)
{
	effect.Setup(*this);
	mesh.Setup(*this);

	pContext->DrawIndexed(mesh.GetIndexCount(), 0, 0);

	effect.Reset();
}

/// <summary>
/// Draws a group of indexed, non-instanced triangle meshes using the given effect
/// </summary>
void Context::Draw(IDynamicArray<Mesh>& meshes, Effect& effect)
{
	effect.Setup(*this);

	for (Mesh& mesh : meshes)
	{
		mesh.Setup(*this);
		pContext->DrawIndexed(mesh.GetIndexCount(), 0, 0);
	}

	effect.Reset();
}

