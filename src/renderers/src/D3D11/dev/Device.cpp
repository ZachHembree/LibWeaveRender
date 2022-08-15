#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/VertexBuffer.hpp"
#include "D3D11/dev/IndexBuffer.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/InputLayout.hpp"
#include "D3D11/dev/VertexShader.hpp"
#include "D3D11/SwapChain.hpp"

using namespace Microsoft::WRL;
using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Device::Device()
{
	GFX_THROW_FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&pDev,
		nullptr,
		&pContext
	));

	IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/// <summary>
/// Returns pointer to COM device interface
/// </summary>
ID3D11Device* Device::Get() const { return pDev.Get(); }

/// <summary>
/// Returns pointer to COM device context interface
/// </summary>
ID3D11DeviceContext* Device::GetContext() const { return pContext.Get(); }

/// <summary>
/// Creates an RT view for accessing resource data
/// </summary>
ComPtr<ID3D11RenderTargetView> Device::GetRtView(const ComPtr<ID3D11Resource>& buffer)
{
	ComPtr<ID3D11RenderTargetView> pRtView;
	GFX_THROW_FAILED(pDev->CreateRenderTargetView(buffer.Get(), nullptr, &pRtView));

	return pRtView;
}

/// <summary>
/// Clears the given render target to the given color
/// </summary>
void Device::ClearRenderTarget(const ComPtr<ID3D11RenderTargetView>& rtView, vec4 color)
{
	pContext->ClearRenderTargetView(rtView.Get(), reinterpret_cast<float*>(&color));
}

/// <summary>
/// Binds the given viewport to the rasterizer stage
/// </summary>
void Device::RSSetViewport(const glm::vec2 size, const glm::vec2 offset, const glm::vec2 depth)
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
void Device::IASetVertexBuffer(VertexBuffer& vertBuffer, int slot)
{
	UINT offset = 0;
	pContext->IASetVertexBuffers(slot, 1, vertBuffer.GetAddressOf(), &vertBuffer.stride, &offset);
}

/// <summary>
/// Determines how vertices are interpreted by the input assembler
/// </summary>
void Device::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	pContext->IASetPrimitiveTopology(topology);
}

/// <summary>
/// Binds the given input layout to the input assembler
/// </summary>
void Device::IASetInputLayout(const InputLayout& vsLayout)
{
	pContext->IASetInputLayout(vsLayout.Get());
}

/// <summary>
/// Binds an array of buffers starting at the given slot
/// </summary>
void Device::IASetVertexBuffers(IDynamicCollection<VertexBuffer>& vertBuffers, int startSlot)
{
	for (int i = 0; i < vertBuffers.GetLength(); i++)
	{
		IASetVertexBuffer(vertBuffers[i], startSlot + i);
	}
}

/// <summary>
/// Binds an index buffer to the input assembler. Used with DrawIndexed().
/// </summary>
void Device::IASetIndexBuffer(IndexBuffer& idxBuf)
{
	pContext->IASetIndexBuffer(idxBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
}

/// <summary>
/// Binds vertex shader to the device 
/// </summary>
void Device::VSSetShader(const VertexShader& vs)
{
	pContext->VSSetShader(vs.Get(), nullptr, 0);
}

/// <summary>
/// Assigns given constant buffer to the given slot
/// </summary>

void Device::VSSetConstantBuffer(ConstantBuffer& buffer, UINT slot)
{
	pContext->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

/// <summary>
/// Binds pixel shader to the device
/// </summary>
void Device::PSSetShader(const ComPtr<ID3D11PixelShader>& pPS)
{
	pContext->PSSetShader(pPS.Get(), nullptr, 0);
}

/// <summary>
/// Binds the given render target to the output merger
/// </summary>
void Device::OMSetRenderTarget(ComPtr<ID3D11RenderTargetView>& pRT)
{
	pContext->OMSetRenderTargets(1, pRT.GetAddressOf(), nullptr);
}

/// <summary>
/// Draw indexed, non-instanced primitives
/// </summary>
void Device::DrawIndexed(UINT length, UINT start, UINT baseVertexLocation)
{
	pContext->DrawIndexed(length, start, baseVertexLocation);
}
