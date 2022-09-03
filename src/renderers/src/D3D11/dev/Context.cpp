#include "D3D11/dev/VertexBuffer.hpp"
#include "D3D11/dev/IndexBuffer.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/InputLayout.hpp"
#include "D3D11/dev/VertexShader.hpp"
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/dev/Context.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Context::Context(Device* pDev, ComPtr<ID3D11DeviceContext>& pContext) :
	DeviceChild(pDev),
	pContext(pContext),
	currentVS(nullptr),
	currentPS(nullptr)
{ }

Context::Context() : 
	DeviceChild(nullptr),
	currentVS(nullptr),
	currentPS(nullptr)
{ }

Context::Context(Context&& other) noexcept :
	DeviceChild(other.pDev),
	pContext(other.pContext),
	currentVS(nullptr),
	currentPS(nullptr)
{
	other.pDev = nullptr;
}

Context& Context::operator=(Context&& other) noexcept
{
	this->pDev = other.pDev;
	this->pContext = std::move(other.pContext);
	other.pDev = nullptr;

	return *this;
}

void Context::SetVS(VertexShader* vs)
{
	if (vs != currentVS || currentVS == nullptr)
	{
		ID3D11VertexShader* pVs = vs != nullptr ? vs->Get() : nullptr;
		Get()->VSSetShader(pVs, nullptr, 0);
		currentVS = vs;
	}
}

void Context::SetPS(PixelShader* ps)
{
	if (ps != currentPS || currentPS == nullptr)
	{ 
		ID3D11PixelShader* pPs = ps != nullptr ? ps->Get() : nullptr;
		Get()->PSSetShader(pPs, nullptr, 0);
		currentPS = ps;
	}
}

bool Context::GetIsVsBound(VertexShader* vs)
{
	return currentVS != nullptr && vs == currentVS;
}

bool Context::GetIsPsBound(PixelShader* ps)
{
	return currentPS != nullptr && ps == currentPS;
}

void Context::Reset()
{
	SetVS(nullptr);
	SetPS(nullptr);
}

/// <summary>
/// Returns pointer to context interface
/// </summary>
ID3D11DeviceContext* Context::Get() const { return pContext.Get(); }

/// <summary>
/// Binds the given viewport to the rasterizer stage
/// </summary>
void Context::RSSetViewport(const vec2 size, const vec2 offset, const vec2 depth)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = size.x;
	vp.Height = size.y;
	vp.TopLeftX = offset.x;
	vp.TopLeftY = offset.y;
	vp.MinDepth = depth.x;
	vp.MaxDepth = depth.y;

	pContext->RSSetViewports(1, &vp);
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
void Context::IASetVertexBuffers(IDynamicCollection<VertexBuffer>& vertBuffers, int startSlot)
{
	for (int i = 0; i < vertBuffers.GetLength(); i++)
	{
		IASetVertexBuffer(vertBuffers[i], startSlot + i);
	}
}

/// <summary>
/// Draw indexed, non-instanced primitives
/// </summary>
void Context::DrawIndexed(UINT length, UINT start, UINT baseVertexLocation)
{
	pContext->DrawIndexed(length, start, baseVertexLocation);
}

