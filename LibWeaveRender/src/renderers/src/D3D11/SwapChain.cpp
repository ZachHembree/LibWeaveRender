#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Renderer.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(SwapChain);

SwapChain::SwapChain(Device& dev) :
	DeviceChild(dev),
	backBufRt(dev, this, &pBackBuffer, &pBackBufRTV),
	isInitialized(false),
	isTearingSupported(false),
	syncMode(VSyncRenderModes::TripleBuffered)
{
	WV_LOG_INFO() << "Swap Chain Init";
	D3D_CHECK_HR(CreateDXGIFactory2(0, __uuidof(IDXGIFactory5), reinterpret_cast<void**>(&pFactory)));

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
	desc.BufferCount = 3; // Triple buffered
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // No blending
	desc.Scaling = DXGI_SCALING_NONE; // Native output, no scaling needed
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	BOOL featValue = 0;

	if (FAILED(pFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &featValue, sizeof(featValue))))
		WV_LOG_WARN() << "Failed to check tearing support";

	isTearingSupported = (bool)featValue;
	desc.Flags = isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
}

void SwapChain::Init()
{
	if (isInitialized)
		return;

	const MinWindow& wnd = GetRenderer().GetWindow();
	Device& dev = *pDev;

	UniqueComPtr<IDXGISwapChain1> pSwapBase;
	D3D_CHECK_HR(pFactory->CreateSwapChainForHwnd(
		dev.Get(),
		wnd.GetWndHandle(),
		&desc,
		&fsDesc,
		NULL,
		&pSwapBase
	));

	D3D_CHECK_HR(pSwapBase.As(&pSwap));
	D3D_ASSERT_HR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer));
	D3D_ASSERT_HR(GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pBackBufRTV));

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
		"\nBuffer Size: " << desc.Width << " x " << desc.Height <<
		"\nFormat: " << GetFormatName((Formats)desc.Format) <<
		"\nAlpha Mode: " << GetAlphaModeName(desc.AlphaMode) <<
		"\nScaling: " << GetScalingModeName(desc.Scaling) <<
		"\nFlags: " << GetSwapChainFlagName((DXGI_SWAP_CHAIN_FLAG)desc.Flags) <<
		"\nTearing Support: " << (isTearingSupported ? "TRUE" : "FALSE") << 
		"\nVSync Mode: " << GetVSyncRenderModeName(syncMode);

	isInitialized = true;
}

bool SwapChain::GetIsInitialized() const { return isInitialized; }

uivec2 SwapChain::GetSize() const { return uivec2(desc.Width, desc.Height); }

uivec2 SwapChain::GetRefresh() const 
{
	if (GetIsFullscreen())
	{
		return uivec2(fsDesc.RefreshRate.Numerator, fsDesc.RefreshRate.Denominator);
	}
	else
	{
		WndMonConfig monCfg = GetRenderer().GetWindow().GetActiveMonitorConfig();
		return uivec2(monCfg.refreshHz, 1u);
	}
}

uint SwapChain::GetBufferCount() const { return desc.BufferCount; }

void SwapChain::SetBufferFormat(Formats format)
{
	if (format == GetBufferFormat())
		return;

	for (DisplayOutput& disp : GetDevice().GetDisplays())
		disp.SetFormat(format);

	desc.Format = (DXGI_FORMAT)format;

	if (isInitialized)
		ApplyDesc();
}

Formats SwapChain::GetBufferFormat() const { return (Formats)desc.Format; }

RTHandle& SwapChain::GetBackBuf() { return backBufRt; }

bool SwapChain::GetIsSyncModeSupported(VSyncRenderModes mode) const
{
	switch (mode)
	{
	case VSyncRenderModes::Disabled:
		return GetIsFullscreen() || (isTearingSupported && GetRenderer().GetWindow().GetIsFullScreen());
	case VSyncRenderModes::VariableRefresh:
		return isTearingSupported && (GetIsFullscreen() || GetRenderer().GetWindow().GetIsFullScreen());
	case VSyncRenderModes::TripleBuffered:
		return true;
	default:
		return false;
	}
}

VSyncRenderModes SwapChain::GetSyncMode() const { return GetIsSyncModeSupported(syncMode) ? syncMode : VSyncRenderModes::TripleBuffered; }

bool SwapChain::TrySetSyncMode(VSyncRenderModes mode)
{
	if (mode == syncMode)
		return true;

	if (!GetIsSyncModeSupported(mode))
	{
		if (mode == VSyncRenderModes::VariableRefresh)
			WV_LOG_WARN() << "Variable rate refresh (VRR) cannot be enabled on platforms without tearing support.";
		else if (mode == VSyncRenderModes::Disabled && !GetIsFullscreen())
			WV_LOG_WARN() << "Disabling VSync requires exclusive full screen or borderless full screen with tearing support";
		else
			WV_LOG_WARN() << "Unsupported VSync configuration set: " << GetVSyncRenderModeName(mode);

		return false;
	}
	else
	{
		switch (mode)
		{
		case VSyncRenderModes::Disabled:
		case VSyncRenderModes::VariableRefresh:
			desc.BufferCount = 2;
			break;
		case VSyncRenderModes::TripleBuffered:
			desc.BufferCount = 3;
			break;
		}

		syncMode = mode;

		if (isInitialized)
			ApplyDesc();

		return true;
	}
}

void SwapChain::ResizeBuffers(uivec2 dim)
{
	if (dim.x == 0 || dim.y == 0) // Transition swap chain to occluded state
	{	
		pBackBuffer.Reset();
		pBackBufRTV.Reset();
		D3D_ASSERT_HR(pSwap->ResizeBuffers(0, 0, 0, (DXGI_FORMAT)Formats::UNKNOWN, desc.Flags));
	}
	else // General purpose resize/disocclude
	{ 
		desc.Width = dim.x;
		desc.Height = dim.y;

		if (isInitialized)
			ApplyDesc();
	}
}

void SwapChain::ApplyDesc()
{
	pBackBuffer.Reset();
	pBackBufRTV.Reset();

	D3D_ASSERT_HR(pSwap->ResizeBuffers(desc.BufferCount, desc.Width, desc.Height, desc.Format, desc.Flags));
	pSwap->GetDesc1(&desc);

	D3D_ASSERT_HR(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer));
	D3D_ASSERT_HR(GetDevice()->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pBackBufRTV));
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
			SetFullscreen(false, true);
			wnd.SetActiveMonitor(newDisp.GetHandle());
			SetFullscreen(true, false);
		}
		else
		{
			wnd.SetActiveMonitor(newDisp.GetHandle());
		}
	}
}

void SwapChain::SetDisplayMode(uivec2 modeID)
{
	const uint dispID = GetDisplayOutput();
	const DisplayOutput& disp = GetDevice().GetDisplays()[dispID];
	const DisplayMode& mode = disp.GetModes()[modeID.x];

	if (GetIsFullscreen())
	{
		const uivec2 refresh = (modeID.y != (uint)-1) ? mode.refreshRates[modeID.y] : uivec2(0);
		desc.Width = mode.resolution.x;
		desc.Height = mode.resolution.y;
		fsDesc.RefreshRate =
		{
			.Numerator = refresh.x,
			.Denominator = refresh.y
		};

		if (isInitialized)
		{
			DXGI_MODE_DESC newDesc =
			{
				.Width = desc.Width,
				.Height = desc.Height,
				.RefreshRate = fsDesc.RefreshRate,
				.ScanlineOrdering = fsDesc.ScanlineOrdering,
				.Scaling = fsDesc.Scaling
			};

			// Set new display mode
			D3D_ASSERT_HR(pSwap->ResizeTarget(&newDesc));

			// Resize buffers
			ApplyDesc();
		}
	}
	else
	{
		ResizeBuffers(mode.resolution);
	}
}

uivec2 SwapChain::GetDisplayMode()
{
	const uint dispID = GetDisplayOutput();
	const DisplayOutput& disp = GetDevice().GetDisplays()[dispID];
	return disp.GetClosestMatch(GetSize(), GetRefresh());
}

bool SwapChain::GetIsFullscreen() const { return !fsDesc.Windowed; }

void SwapChain::SetFullscreen(bool isFullscreen, bool isOccluded)
{
	MinWindow& wnd = GetRenderer().GetWindow();
	fsDesc.Windowed = !isFullscreen;

	if (isInitialized)
	{
		if (!isOccluded && isFullscreen)
			wnd.SetFullScreen(true);

		// Enable/disable exclusive full screen mode
		D3D_ASSERT_HR(pSwap->SetFullscreenState(isFullscreen, nullptr));

		// Update buffers
		if (isOccluded)
			// Handle occlusion/focus loss in exclusive full screen
			ResizeBuffers(uivec2(0));
		else if (isFullscreen)
			// Use explicit display modes for exclusive full screen
			SetDisplayMode(GetDisplayMode());
		else // Use current monitor resolution for windowed modes
			ResizeBuffers(wnd.GetMonitorResolution());
	
		if (!isOccluded && !isFullscreen)
			wnd.SetFullScreen(false);
	}
}

void SwapChain::Present(uint syncInterval)
{
	D3D_ASSERT_MSG(isInitialized, "Cannot call present on uninitialized swap chain.");
	
	if (GetIsSyncModeSupported(syncMode))
	{
		if (syncMode == VSyncRenderModes::VariableRefresh)
			pSwap->Present(0, !GetIsFullscreen() ? DXGI_PRESENT_ALLOW_TEARING : 0);
		else if (syncMode == VSyncRenderModes::Disabled)
			pSwap->Present(0, (isTearingSupported && !GetIsFullscreen()) ? DXGI_PRESENT_ALLOW_TEARING : 0);
		else
			pSwap->Present(syncInterval, 0);
	}
	else
		pSwap->Present(syncInterval, 0);
}