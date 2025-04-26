#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Renderer.hpp"

using namespace Weave;
using namespace Weave::D3D11;

SwapChain::SwapChain() : desc({}), fsDesc({}), isInitialized(false)
{ }

SwapChain::SwapChain(Device& dev) :
	DeviceChild(dev),
	backBufRt(dev, this, &pBackBuffer, &pBackBufRTV),
	isInitialized(false)
{
	WV_LOG_INFO() << "Swap Chain Init";
	D3D_CHECK_HR(CreateDXGIFactory1(__uuidof(IDXGIFactory2), &dxgiFactory));

	// Set defaults
	fsDesc = {};
	fsDesc.RefreshRate.Denominator = 0; // Use current refresh rate
	fsDesc.RefreshRate.Numerator = 0;
	fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Native output, no scaling needed
	fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // No interlacing
	fsDesc.Windowed = TRUE;

	desc.Width = 0;
	desc.Height = 0;
	desc.Format = (DXGI_FORMAT)Formats::UNKNOWN;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1; // No MSAA
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2; // Triple buffered
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // No blending
	desc.Scaling = DXGI_SCALING_NONE; // Native output, no scaling needed
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
}

void SwapChain::Init()
{
	if (isInitialized)
		return;

	const MinWindow& wnd = GetRenderer().GetWindow();
	Device& dev = *pDev;

	D3D_CHECK_HR(dxgiFactory->CreateSwapChainForHwnd(
		dev.Get(),
		wnd.GetWndHandle(),
		&desc,
		&fsDesc,
		NULL,
		&pSwap
	));

	GetBuffers();

	const DisplayOutput* pActive = nullptr;
	
	if (const uint activeIndex = GetDisplayOutput(); activeIndex != (uint)-1)
		pActive = &GetDevice().GetDisplays()[activeIndex];

	if (pActive != nullptr)
		WV_LOG_INFO() << "Active Monitor: " << pActive->GetName();
	else
		WV_LOG_INFO() << "No active monitor found.";

	WV_LOG_INFO() <<
		"Swap Chain Configuration" <<
		"\nWindowed Mode: " << (fsDesc.Windowed ? "TRUE" : "FALSE") <<
		"\nOutput Size: " << wnd.GetBodySize().x << " x " << wnd.GetBodySize().y <<
		"\nBuffer Size: " << desc.Width << " x " << desc.Height <<
		"\nFormat: " << GetFormatName((Formats)desc.Format) <<
		"\nAlpha Mode: " << GetAlphaModeName(desc.AlphaMode) <<
		"\nScaling: " << GetScalingModeName(desc.Scaling) <<
		"\nFlags: " << GetSwapChainFlagName((DXGI_SWAP_CHAIN_FLAG)desc.Flags);

	isInitialized = true;
}

bool SwapChain::GetIsInitialized() const { return isInitialized; }

IDXGISwapChain1& SwapChain::Get() { return *pSwap.Get(); }

IDXGISwapChain1** const SwapChain::GetAddressOf() { return pSwap.GetAddressOf(); }

IDXGISwapChain1* SwapChain::operator->() { return pSwap.Get(); }

uivec2 SwapChain::GetSize() const { return uivec2(desc.Width, desc.Height); }

uint SwapChain::GetBufferCount() const { return desc.BufferCount; }

void SwapChain::SetBufferFormat(Formats format)
{
	if (format == GetBufferFormat())
		return;

	for (DisplayOutput& disp : GetDevice().GetDisplays())
		disp.SetFormat(format);

	desc.Format = (DXGI_FORMAT)format;

	if (isInitialized)
		pSwap->ResizeBuffers(desc.BufferCount, desc.Width, desc.Height, desc.Format, desc.Flags);
}

Formats SwapChain::GetBufferFormat() const { return (Formats)desc.Format; }

RTHandle& SwapChain::GetBackBuf() { return backBufRt; }

void SwapChain::ResizeBuffers(uivec2 dim, uint count)
{
	pBackBuffer.Reset();
	pBackBufRTV.Reset();

	if (dim.x == 0 || dim.y == 0) // Transition swap chain to occluded state
	{	
		pSwap->ResizeBuffers(0, 0, 0, (DXGI_FORMAT)Formats::UNKNOWN, desc.Flags);
		GetBuffers();
	}
	else // General purpose resize/disocclude
	{ 
		if (count == 0)
			count = GetBufferCount();

		desc.BufferCount = count;
		desc.Width = dim.x;
		desc.Height = dim.y;

		if (isInitialized)
		{
			pSwap->ResizeBuffers(desc.BufferCount, desc.Width, desc.Height, desc.Format, desc.Flags);
			pSwap->GetDesc1(&desc);
			GetBuffers();
		}
	}
}

uint SwapChain::GetDisplayOutput() const
{
	// Find active output
	HMONITOR monHandle = GetRenderer().GetWindow().GetActiveMonitor();
	const IDynamicArray<DisplayOutput>& displays = GetDevice().GetDisplays();
	const DisplayOutput* pActive = nullptr;

	for (uint i = 0; i < displays.GetLength(); i++)
	{
		if (displays[i].GetHandle() == monHandle)
			return i;
	}

	return (uint)-1;
}

void SwapChain::SetDisplayOutput(uint index)
{
	const uint currentOutput = GetDisplayOutput();

	if (index != currentOutput)
	{
		MinWindow& wnd = GetRenderer().GetWindow();
		const IDynamicArray<DisplayOutput>& displays = GetDevice().GetDisplays();
		const DisplayOutput& newDisp = displays[index];

		if (GetIsFullscreen())
		{
			SetFullscreen(false, false);
			wnd.SetActiveMonitor(newDisp.GetHandle());
			ResizeBuffers(GetRenderer().GetOutputResolution());
			SetFullscreen(true, false);
		}
		else
		{
			wnd.SetActiveMonitor(newDisp.GetHandle());
		}
	}
}

bool SwapChain::GetIsFullscreen() const { return !fsDesc.Windowed; }

void SwapChain::SetFullscreen(bool isFullscreen, bool isOccluded)
{
	if (isFullscreen == GetIsFullscreen())
		return;

	fsDesc.Windowed = !isFullscreen;

	if (isInitialized)
	{
		D3D_ASSERT_HR(pSwap->SetFullscreenState(isFullscreen, nullptr));

		// Handle occlusion/focus loss in exclusive full screen
		if (isFullscreen && isOccluded)
			ResizeBuffers(uivec2(0));
		else // Call resize on mode change with current size
			ResizeBuffers(GetRenderer().GetWindow().GetMonitorResolution());
	}
}

void SwapChain::Present(CtxImm& ctx, uint syncInterval, uint flags)
{
	D3D_ASSERT_MSG(isInitialized, "Cannot call present on uninitialized swap chain.");
	ctx.Present(*this, syncInterval, flags);
}

void SwapChain::GetBuffers()
{
	D3D_ASSERT_HR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer));
	D3D_ASSERT_HR(GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pBackBufRTV));
}