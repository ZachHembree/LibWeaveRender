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
}

/// <summary>
/// Returns pointer to context interface
/// </summary>
ID3D11DeviceContext& Context::Get() const { return *pContext.Get(); }

/// <summary>
/// Returns reference to context interface
/// </summary>
ID3D11DeviceContext* Context::operator->() const { return pContext.Get(); }

void Context::RSSetViewports(const IDynamicArray<Viewport>& viewports, int offset)
{
	GFX_ASSERT((offset + viewports.GetLength()) <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		"Number of viewports supplied exceeds limit.")

	size_t vpStart = sizeof(Viewport) * offset;
	memcpy(currentVPs.GetPtr() + vpStart, viewports.GetPtr(), viewports.GetSize());

	vpCount = std::max(vpCount, (uint)(offset + viewports.GetLength()));
	pContext->RSSetViewports(vpCount, (D3D11_VIEWPORT*)currentVPs.GetPtr());
}

/// <summary>
/// Binds the given viewport to the rasterizer stage
/// </summary>
void Context::RSSetViewport(int index, const vec2 size, const vec2 offset, const vec2 depth)
{
	GFX_ASSERT(index >= 0 && (index) < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE,
		"Viewport index out of range.")

	vpCount = std::max(vpCount, (uint)(index + 1));
	currentVPs[index] = { offset, size, depth };
	pContext->RSSetViewports(vpCount, (D3D11_VIEWPORT*)currentVPs.GetPtr());
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

	// Update viewports for render textures
	Viewport viewports[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]({});

	for (int i = 0; i < (int)rtvCount; i++)
	{
		viewports[i] = 
		{ 
			rtvs[i].GetOffset(), 
			rtvs[i].GetRenderSize(), 
			currentDepthRange 
		};
	}

	RSSetViewports(Span((Viewport*)&viewports, rtvCount));
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

bool CanDirectCopy(ITexture2D& src, ITexture2D& dst)
{
	return 
		dst.GetUsage() != ResourceUsages::Immutable &&
		src.GetSize() == dst.GetSize() &&
		src.GetFormat() == dst.GetFormat();
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRWTexture2D& dst)
{
	if (CanDirectCopy(src, dst))
	{
		pContext->CopyResource(dst.GetResource(), src.GetResource());
	}
	else if (src.GetSize() == dst.GetSize())
	{
		Renderer& renderer = GetRenderer();
		ComputeShader& cs = renderer.GetDefaultCompute(L"TexCopy2D");

		cs.SetTexture(L"SrcTex", src);
		cs.SetRWTexture(L"DstTex", dst);
		cs.Dispatch(*this, ivec3(dst.GetSize(), 1));
	}
	else
	{ 
		Renderer& renderer = GetRenderer();
		ComputeShader& cs = renderer.GetDefaultCompute(L"TexCopySamp2D");

		cs.SetTexture(L"SrcTex", src);
		cs.SetRWTexture(L"DstTex", dst);
		cs.SetSampler(L"Samp", renderer.GetDefaultSampler(L"LinearClamp"));
		cs.SetConstant(L"DstTexelSize", dst.GetTexelSize());
		cs.Dispatch(*this, ivec3(dst.GetSize(), 1));
	}
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, ITexture2D& dst)
{
	if (CanDirectCopy(src, dst))
	{
		pContext->CopyResource(dst.GetResource(), src.GetResource());
	}
	else
	{
		GFX_THROW("Failed to copy texture. Destination incompatible with source.");
	}
}

/// <summary>
/// Copies the contents of one texture to another
/// </summary>
void Context::Blit(ITexture2D& src, IRenderTarget& dst)
{
	Renderer& renderer = GetRenderer();
	Mesh& quad = renderer.GetDefaultMesh(L"FSQuad");
	Effect& quadFX = renderer.GetDefaultEffect(L"PosTextured2D");

	pContext->OMSetRenderTargets(1, dst.GetAddressRTV(), nullptr);
	quadFX.SetTexture(L"tex", src);
	quadFX.SetSampler(L"samp", renderer.GetDefaultSampler(L"LinearClamp"));
	Draw(quad, quadFX);

	pContext->OMSetRenderTargets(rtvCount, currentRTVs.GetPtr(), currentDSV);
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

