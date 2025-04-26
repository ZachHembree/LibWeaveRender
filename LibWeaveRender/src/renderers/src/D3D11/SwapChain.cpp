#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Renderer.hpp"

using namespace Weave;
using namespace Weave::D3D11;

SwapChain::SwapChain() : desc({})
{ }

SwapChain::SwapChain(const MinWindow& wnd, Device& dev) :
	DeviceChild(dev),
	backBufRt(dev, this, &pBackBuffer, &pBackBufRTV)
{
	WV_LOG_INFO() << "Swap Chain Init";

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
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	D3D_CHECK_HR(dxgiFactory->CreateSwapChainForHwnd(
		dev.Get(),
		wnd.GetWndHandle(),
		&desc,
		&fsDesc,
		NULL,
		&pSwap
	));

	GetBuffers();
	
	WV_LOG_INFO() << 
		"Swap Chain Configuration" <<
		"\nWindowed Mode: " << (fsDesc.Windowed ? "TRUE" : "FALSE") <<
		"\nWindow Size: " << wnd.GetBodySize().x << " x " << wnd.GetBodySize().y <<
		"\nBuffer Size: " << monRes.x << " x " << monRes.y <<
		"\nFormat: " << GetFormatName((Formats)desc.Format) <<
		"\nAlpha Mode: " << GetAlphaModeName(desc.AlphaMode) <<
		"\nScaling: " << GetScalingModeName(desc.Scaling) << 
		"\nFlags: " << GetSwapChainFlagName((DXGI_SWAP_CHAIN_FLAG)desc.Flags);
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

uivec2 SwapChain::GetSize() const
{
	ivec2 size(desc.Width, desc.Height);
	return size;
}

uint SwapChain::GetBufferCount() const
{
	return desc.BufferCount;
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

void SwapChain::ResizeBuffers(uivec2 dim, uint count, Formats format, uint flags)
{
	pBackBuffer.Reset();
	pBackBufRTV.Reset();

	if (flags == 0)
		flags = desc.Flags;

	if (dim.x == 0 || dim.y == 0) // Transition swap chain to occluded state
	{	
		pSwap->ResizeBuffers(0, 0, 0, (DXGI_FORMAT)Formats::UNKNOWN, flags);
	}
	else // General purpose resie/disocclude
	{ 
		if (count == 0)
			count = GetBufferCount();

		if (format == Formats::UNKNOWN)
			format = GetBufferFormat();

		pSwap->ResizeBuffers(count, dim.x, dim.y, (DXGI_FORMAT)format, flags);
		pSwap->GetDesc1(&desc);
	}

	GetBuffers();
}

bool SwapChain::GetIsFullscreen() const 
{
	BOOL isFullscreen;
	D3D_ASSERT_HR(pSwap->GetFullscreenState(&isFullscreen, nullptr));
	return isFullscreen > 0;
}

void SwapChain::SetFullscreen(bool isFullscreen, bool isOccluded)
{
	if (isFullscreen == GetIsFullscreen())
		return;

	D3D_ASSERT_HR(pSwap->SetFullscreenState(isFullscreen, nullptr));

	// Handle occlusion/focus loss in exclusive full screen
	if (isFullscreen && isOccluded)
		ResizeBuffers(uivec2(0));
	else // Call resize on mode change with current size
		ResizeBuffers(GetSize());
}

void SwapChain::Present(CtxImm& ctx, uint syncInterval, uint flags)
{
	ctx.Present(*this, syncInterval, flags);
}

void SwapChain::GetBuffers()
{
	D3D_ASSERT_HR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer));
	D3D_ASSERT_HR(GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pBackBufRTV));
}