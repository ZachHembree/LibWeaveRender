#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "WeaveRender/D3D11.hpp"
#include "D3D11/Device.hpp"

using namespace glm;
using namespace Weave;
using namespace Weave::D3D11;

Device::Device(Renderer& renderer) : pRenderer(&renderer)
{
	ComPtr<ID3D11DeviceContext> pCtx;
	ComPtr<ID3D11Device> pDevBase;
	D3D_CHECK_HR(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		g_DeviceFlags,
		&g_FeatureLevel,
		1,
		D3D11_SDK_VERSION,
		&pDevBase,
		nullptr,
		&pCtx
	));

	D3D_CHECK_HR(pDevBase->QueryInterface(__uuidof(ID3D11Device1), &pDev));
	context = Context(*this, std::move(pCtx));
}

Renderer& Device::GetRenderer() const { D3D_ASSERT_MSG(pRenderer != nullptr, "Renderer cannot be null."); return *pRenderer; }

/// <summary>
/// Returns reference to COM device interface
/// </summary>
ID3D11Device1& Device::Get() { return *pDev.Get(); }

/// <summary>
/// Returns reference to COM device interface
/// </summary>
ID3D11Device1* Device::operator->() { return pDev.Get(); }

/// <summary>
/// Returns reference to main device context
/// </summary>
Context& Device::GetContext() { return this->context; }

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11VertexShader>& pVS)
{
	D3D_CHECK_HR(pDev->CreateVertexShader(binSrc.GetData(), binSrc.GetLength(), nullptr, &pVS));
}

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11PixelShader>& pPS)
{
	D3D_CHECK_HR(pDev->CreatePixelShader(binSrc.GetData(), binSrc.GetLength(), nullptr, &pPS));
}

void Device::CreateShader(const IDynamicArray<byte>& binSrc, ComPtr<ID3D11ComputeShader>& pCS)
{
	D3D_CHECK_HR(pDev->CreateComputeShader(binSrc.GetData(), binSrc.GetLength(), nullptr, &pCS));
}