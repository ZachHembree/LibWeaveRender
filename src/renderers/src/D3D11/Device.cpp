#include "D3D11/Device.hpp"
#include "D3D11/VertexBuffer.hpp"
#include "D3D11/IndexBuffer.hpp"
#include "D3D11/ConstantBuffer.hpp"
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
/// Binds an array of buffers starting at the given slot
/// </summary>
void Device::IASetVertexBuffers(DynamicArrayBase<VertexBuffer>& vertBuffers, int startSlot)
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
/// Creates a vertex shader object from compiled shader
/// </summary>
ComPtr<ID3D11VertexShader> Device::CreateVertexShader(const ComPtr<ID3DBlob>& vsBlob)
{
	ComPtr<ID3D11VertexShader> pVS;
	GFX_THROW_FAILED(pDev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));
	return pVS;
}

/// <summary>
/// Binds vertex shader to the device 
/// </summary>
void Device::VSSetShader(const ComPtr<ID3D11VertexShader>& vertexShader)
{
	pContext->VSSetShader(vertexShader.Get(), nullptr, 0);
}

/// <summary>
/// Assigns given constant buffer to the given slot
/// </summary>

void Replica::D3D11::Device::VSSetConstantBuffer(ConstantBuffer& buffer, UINT slot)
{
	pContext->VSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
}

/// <summary>
/// Creates a pixel shader object from a compiled shader
/// </summary>
ComPtr<ID3D11PixelShader> Device::CreatePixelShader(const ComPtr<ID3DBlob>& psBlob)
{
	ComPtr<ID3D11PixelShader> pPS;
	GFX_THROW_FAILED(pDev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
	return pPS;
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
