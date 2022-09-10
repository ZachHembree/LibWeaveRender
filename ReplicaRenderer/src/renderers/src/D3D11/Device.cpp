#include "D3D11/Device.hpp"
#include "D3D11/Renderer.hpp"
#include "GfxException.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Device::Device(Renderer& renderer) : pRenderer(&renderer)
{
	const D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_1;

	ComPtr<ID3D11DeviceContext> pContext;
	GFX_THROW_FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		&level,
		1,
		D3D11_SDK_VERSION,
		&pDev,
		nullptr,
		&pContext
	));

	context = Context(*this, pContext);
	context.IASetPrimitiveTopology(PrimTopology::TRIANGLELIST);
}

Renderer& Device::GetRenderer() { GFX_ASSERT(pRenderer != nullptr, "Renderer cannot be null."); return *pRenderer; }

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