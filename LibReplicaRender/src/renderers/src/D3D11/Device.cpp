#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "ReplicaRender/D3D11.hpp"
#include "D3D11/Device.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

Device::Device(Renderer& renderer) : pRenderer(&renderer)
{
	ComPtr<ID3D11DeviceContext> pContext;
	GFX_THROW_FAILED(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		g_DeviceFlags,
		&g_FeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&pDev,
		nullptr,
		&pContext
	));

	context = Context(*this, pContext);
}

Renderer& Device::GetRenderer() const { GFX_ASSERT(pRenderer != nullptr, "Renderer cannot be null."); return *pRenderer; }

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

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11VertexShader>& pVS)
{
	GFX_THROW_FAILED(pDev->CreateVertexShader(binSrc.GetPtr(), binSrc.GetLength(), nullptr, &pVS));
}

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11PixelShader>& pPS)
{
	GFX_THROW_FAILED(pDev->CreatePixelShader(binSrc.GetPtr(), binSrc.GetLength(), nullptr, &pPS));
}

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11ComputeShader>& pCS)
{
	GFX_THROW_FAILED(pDev->CreateComputeShader(binSrc.GetPtr(), binSrc.GetLength(), nullptr, &pCS));
}