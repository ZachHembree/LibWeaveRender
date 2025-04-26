#include "pch.hpp"
#include <iomanip>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/DisplayOutput.hpp"

using namespace glm;
using namespace Weave;
using namespace Weave::D3D11;

Device::Device(Renderer& renderer) : pRenderer(&renderer)
{
	WV_LOG_INFO() << "Device Init";

	ComPtr<ID3D11DeviceContext> pCtxBase;
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
		&pCtxBase
	));

	D3D_CHECK_HR(pDevBase.As(&pDev));
	D3D_CHECK_HR(pCtxBase.As(&pCtxImm));

	ComPtr<ID3D11DeviceContext1> pCtxCpy = pCtxImm;
	context = CtxImm(*this, std::move(pCtxCpy));

	// Get device info
	ComPtr<IDXGIDevice1> pDxgiDev;
	ComPtr<IDXGIAdapter> pAdapterBase;
	ComPtr<IDXGIAdapter1> pAdapter;
	D3D_CHECK_HR(pDev.As(&pDxgiDev));
	D3D_CHECK_HR(pDxgiDev->GetAdapter(&pAdapterBase));
	D3D_CHECK_HR(pAdapterBase.As(&pAdapter));
	
	DXGI_ADAPTER_DESC1 adapterDesc;
	D3D_CHECK_HR(pAdapter->GetDesc1(&adapterDesc));
	GetMultiByteString_UTF16LE_TO_UTF8(adapterDesc.Description, name);

	ComPtr<IDXGIOutput> pDispBase;

	for (uint i = 0; pAdapter->EnumOutputs(i, pDispBase.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++)
	{
		ComPtr<IDXGIOutput1> pDisp;
		pDispBase.As(&pDisp);
		displays.emplace_back(DisplayOutput(*this, std::move(pDisp), std::format("Display {} ({})", i, name)));
	}

	WV_LOG_INFO() << 
		"D3D Device Information" <<
		"\nFeature Level: " << GetFeatureLevelName(pDev->GetFeatureLevel()) <<
		"\nDebug: " << ((pDev->GetCreationFlags() & D3D11_CREATE_DEVICE_DEBUG) ? "TRUE" : "FALSE") <<
		"\nAdapter: " << name <<
		"\nVendor ID: 0x" << std::hex << adapterDesc.VendorId << std::dec <<
		"\nDevice ID: 0x" << std::hex << adapterDesc.DeviceId << std::dec <<
		"\nSubSys ID: 0x" << std::hex << adapterDesc.SubSysId << std::dec <<
		"\nRevision: " << adapterDesc.Revision <<
		"\nDedicated Video Memory: " << (adapterDesc.DedicatedVideoMemory / (1024 * 1024)) << " MB" <<
		"\nDedicated System Memory: " << (adapterDesc.DedicatedSystemMemory / (1024 * 1024)) << " MB" <<
		"\nShared System Memory: " << (adapterDesc.SharedSystemMemory / (1024 * 1024)) << " MB";
}

Renderer& Device::GetRenderer() const { D3D_ASSERT_MSG(pRenderer != nullptr, "Renderer cannot be null."); return *pRenderer; }

ID3D11Device1* Device::Get() { return pDev.Get(); }

ID3D11Device1* Device::operator->() { return pDev.Get(); }

ID3D11DeviceContext1* Device::GetImmediateContext() { return pCtxImm.Get(); }

CtxImm& Device::GetContext() { return this->context; }

string_view Device::GetAdapterName() const { return name; }

IDynamicArray<DisplayOutput>& Device::GetDisplays() { return displays; }

const IDynamicArray<DisplayOutput>& Device::GetDisplays() const { return displays; }

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