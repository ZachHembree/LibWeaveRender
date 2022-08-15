#include "D3D11/dev/Device.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Device::Device()
{
	ComPtr<ID3D11DeviceContext> pContext;
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

	context = Context(this, pContext);
	context.IASetPrimitiveTopology(PrimTopology::TRIANGLELIST);
}

/// <summary>
/// Returns pointer to COM device interface
/// </summary>
ID3D11Device* Device::Get() { return pDev.Get(); }

/// <summary>
/// Returns reference to device context
/// </summary>
Context& Device::GetContext() { return this->context; }

/// <summary>
/// Creates an RT view for accessing resource data
/// </summary>
ComPtr<ID3D11RenderTargetView> Device::GetRtView(const ComPtr<ID3D11Resource>& buffer)
{
	ComPtr<ID3D11RenderTargetView> pRtView;
	GFX_THROW_FAILED(pDev->CreateRenderTargetView(buffer.Get(), nullptr, &pRtView));

	return pRtView;
}