#include "D3D11/Device.hpp"

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

	context = Context(*this, pContext);
	context.IASetPrimitiveTopology(PrimTopology::TRIANGLELIST);
}

/// <summary>
/// Returns reference to COM device interface
/// </summary>
ID3D11Device& Device::Get() { return *pDev.Get(); }

/// <summary>
/// Returns reference to COM device interface
/// </summary>
ID3D11Device* Device::operator->() { return pDev.Get(); }

/// <summary>
/// Returns reference to main device context
/// </summary>
Context& Device::GetContext() { return this->context; }