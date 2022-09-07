#include <d3d11.h>
#include <wrl.h>
#include "MinWindow.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Resources/RenderTarget.hpp"
#include "D3D11/Resources/Texture2D.hpp"

using namespace Replica::D3D11;

SwapChain::SwapChain() : pDev(nullptr), desc({})
{ }

SwapChain::SwapChain(const MinWindow& wnd, Device& dev) :
	pDev(&dev),
	backBufRt(dev, &pBackBuf)
{
	ComPtr<IDXGIFactory2> dxgiFactory;
	GFX_THROW_FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), &dxgiFactory));

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

	GFX_THROW_FAILED(dxgiFactory->CreateSwapChainForHwnd(
		pDev->Get(),
		wnd.GetWndHandle(),
		&desc,
		&fsDesc,
		NULL,
		&pSwap
	));

	GetBuffers();
}

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
/// Returns interface to swap chain buffer at the given index
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
	GFX_THROW_FAILED(pSwap->Present(syncInterval, flags));
}

void SwapChain::GetBuffers()
{
	ComPtr<ID3D11Resource> pRes;
	GFX_THROW_FAILED(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pRes));
	GFX_THROW_FAILED(pDev->Get()->CreateRenderTargetView(pRes.Get(), nullptr, &pBackBuf));
}