#include "pch.hpp"
#include <d3d11.h>
#include <wrl.h>
#include "ReplicaUtils/Win32.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/SwapChain.hpp"

using namespace Replica;
using namespace Replica::D3D11;

SwapChain::SwapChain() : desc({})
{ }

SwapChain::SwapChain(const MinWindow& wnd, Device& dev) :
	DeviceChild(dev),
	backBufRt(dev, this, &pBackBuf)
{
	ComPtr<IDXGIFactory2> dxgiFactory;
	D3D_CHECK_HR(CreateDXGIFactory1(__uuidof(IDXGIFactory2), &dxgiFactory));

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.RefreshRate.Denominator = 0; // Use current refresh rate
	fsDesc.RefreshRate.Numerator = 0;
	fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Native output, no scaling needed
	fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // No interlacing
	fsDesc.Windowed = TRUE;

	const ivec2 monRes = wnd.GetMonitorResolution();
	desc.Width = monRes.x; // Initialize chain to active monitor resolution
	desc.Height = monRes.y;
	desc.Format = (DXGI_FORMAT)Formats::R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1; // No MSAA
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2; // Triple buffered
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // No blending
	desc.Scaling = DXGI_SCALING_NONE; // Native output, no scaling needed
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Flags = 0;

	D3D_CHECK_HR(dxgiFactory->CreateSwapChainForHwnd(
		&dev.Get(),
		wnd.GetWndHandle(),
		&desc,
		&fsDesc,
		NULL,
		&pSwap
	));

	GetBuffers();
}

/// <summary>
/// Returns pointer to swap chain interface
/// </summary>
IDXGISwapChain1& SwapChain::Get() { return *pSwap.Get(); }

/// <summary>
/// Returns read-only pointer to pointer for swap chain interface
/// </summary>
IDXGISwapChain1** const SwapChain::GetAddressOf() { return pSwap.GetAddressOf(); }

/// <summary>
/// Returns pointer to swap chain interface
/// </summary>
IDXGISwapChain1* SwapChain::operator->() { return pSwap.Get(); }

ivec2 SwapChain::GetSize() const
{
	ivec2 size(desc.Width, desc.Height);
	return size;
}

int SwapChain::GetBufferCount() const
{
	return desc.BufferCount + 1;
}

Formats SwapChain::GetBufferFormat() const
{
	return (Formats)desc.Format;
}

/// <summary>
/// Returns handle to swap chain back buffer
/// </summary>
RTHandle& SwapChain::GetBackBuf()
{
	return backBufRt;
}

void SwapChain::ResizeBuffers(ivec2 dim, uint count, Formats format, uint flags)
{
	if (count == 0)
		count = GetBufferCount();

	if (format == Formats::UNKNOWN)
		format = GetBufferFormat();

	if (flags == 0)
		flags = desc.Flags;

	pBackBuf.Reset();
	pSwap->ResizeBuffers(count, dim.x, dim.y, (DXGI_FORMAT)format, flags);
	pSwap->GetDesc1(&desc);
	GetBuffers();
}

/// <summary>
/// Presents rendered image with the given synchronization settings
/// </summary>
void SwapChain::Present(UINT syncInterval, UINT flags)
{
	D3D_ASSERT_HR(pSwap->Present(syncInterval, flags));
}

void SwapChain::GetBuffers()
{
	ComPtr<ID3D11Resource> pRes;
	D3D_ASSERT_HR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pRes));
	D3D_ASSERT_HR(GetDevice()->CreateRenderTargetView(pRes.Get(), nullptr, &pBackBuf));
}