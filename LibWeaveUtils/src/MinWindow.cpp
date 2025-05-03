#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include <ShellScalingApi.h>

#pragma comment(lib, "Shcore.lib")

using namespace glm;
using namespace Weave;

MinWindow::MinWindow(
	wstring_view initName, 
	ivec2 initSize, 
	WndStyle initStyle, 
	const HINSTANCE hInst, 
	const wchar_t* iconRes
) :
	name(initName),
	hInst(hInst),
	initStyle(initStyle),
	hWnd(nullptr),
	wndMsg(MSG{}),
	bodySize(0),
	wndSize(0),
	isInitialized(false),
	isMousedOver(false),
	isFullscreen(false),
	lastPos(0),
	lastSize(0),
	paddingOverride(0),
	canSysSleep(true),
	canDispSleep(true),
	isHeaderHovered(false),
	isNcCustom(false)
{
	// Setup window descriptor
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC; // Own device context
	wc.lpfnWndProc = &HandleWindowSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = (HICON)LoadImage(hInst, iconRes, IMAGE_ICON, 64, 64, 0);
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = initName.data();
	wc.hIconSm = (HICON)LoadImage(hInst, iconRes, IMAGE_ICON, 32, 32, 0);

	// Register window class
	WIN_ASSERT_NZ_LAST(RegisterClassEx(&wc));
	
	RECT wr;
	wr.left = 50;
	wr.right = initSize.x + wr.left;
	wr.top = 50;
	wr.bottom = initSize.y + wr.top;

	// Resize body
	WIN_ASSERT_NZ_LAST(AdjustWindowRect(&wr, initStyle.x, FALSE));

	// Create window instance
	hWnd = CreateWindowExW(
		initStyle.y,
		initName.data(),
		initName.data(),
		initStyle.x,
		// Default starting position
		CW_USEDEFAULT, CW_USEDEFAULT,
		// Starting size
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr, nullptr,
		hInst,
		this
	);

	// Check if window creation failed
	WIN_CHECK_LAST(hWnd != NULL);

	// Make the window visible
	WIN_ASSERT_NZ_LAST(ShowWindow(hWnd, SW_SHOW));

	// Setup mouse tracker
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = hWnd;
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;
}

MinWindow::~MinWindow()
{
	if (hWnd != nullptr)
	{
		isInitialized = false;
		UnregisterClass(name.data(), hInst);
		DestroyWindow(hWnd);
	}
}

const wstring_view MinWindow::GetName() const noexcept
{
	return name;
}

HINSTANCE MinWindow::GetProcHandle() const noexcept
{
	return hInst;
}

HWND MinWindow::GetWndHandle() const noexcept
{
	return hWnd;
}

ivec2 MinWindow::GetOverridePadding() const { return paddingOverride; }

void MinWindow::SetOverridePadding(ivec2 padding) { paddingOverride = padding; }

bool MinWindow::GetIsNonClientCustom() const { return isNcCustom; }

void MinWindow::SetIsNonClientCustom(bool value) 
{ 
	if (value == isNcCustom)
		return;

	isNcCustom = value;
	WIN_CHECK_NZ_LAST(SetWindowPos(
		hWnd,
		HWND_TOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

void MinWindow::HoverHeader(bool isHovered) { isHeaderHovered = isHovered; }

void MinWindow::Minimize() 
{
	PostMessageW(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void MinWindow::Maximize()
{ 
	if (IsZoomed(hWnd))
		WIN_CHECK_NZ_LAST(PostMessageW(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0));
	else
		WIN_CHECK_NZ_LAST(PostMessageW(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0));
}

void MinWindow::CloseWindow() 
{
	PostMessageW(hWnd, WM_CLOSE, 0, 0);
}

wstring MinWindow::GetWindowTitle() const
{
	size_t len = GetWindowTextLengthW(hWnd);
	wstring title(len, '\0');
	WIN_ASSERT_NZ_LAST(GetWindowTextW(hWnd, title.data(), (int)len));

	return title;
}

void MinWindow::SetWindowTitle(wstring_view text)
{
	WIN_CHECK_NZ_LAST(SetWindowTextW(hWnd, text.data()));
}

WndStyle MinWindow::GetStyle() const
{
	WndStyle style;

	style.x = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
	WIN_ASSERT_NZ_LAST(style.x);

	style.y = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	WIN_ASSERT_NZ_LAST(style.y);

	return style;
}

void MinWindow::SetStyle(WndStyle style)
{
	WIN_CHECK_NZ_LAST(SetWindowLongPtr(hWnd, GWL_STYLE, style.x));

	if (style.y != 0L)
		WIN_CHECK_NZ_LAST(SetWindowLongPtr(hWnd, GWL_EXSTYLE, style.y));

	// Update to reflect changes
	WIN_CHECK_NZ_LAST(SetWindowPos(
		hWnd,
		HWND_TOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

ivec2 MinWindow::GetPos() const
{
	RECT rect;
	WIN_CHECK_NZ_LAST(GetWindowRect(hWnd, &rect));
	return ivec2(rect.left, rect.top);
}

void MinWindow::SetPos(ivec2 pos)
{
	WIN_CHECK_NZ_LAST(SetWindowPos(
		hWnd,
		0,
		pos.x, pos.y,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

ivec2 MinWindow::GetBodyPos() const 
{
	POINT pos = {};
	WIN_CHECK_NZ_LAST(ClientToScreen(hWnd, &pos));
	return ivec2((sint)pos.x, (sint)pos.y);
}

ivec2 MinWindow::GetSize() const
{
	return wndSize;
}

void MinWindow::SetSize(ivec2 size)
{
	WIN_CHECK_NZ_LAST(SetWindowPos(
		hWnd,
		HWND_TOP,
		0, 0,
		size.x, size.y,
		SWP_NOREPOSITION | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

ivec2 MinWindow::GetBodySize() const
{
	return bodySize;
}

void MinWindow::SetBodySize(ivec2 size)
{
	ivec2 borderSize = wndSize - bodySize;
	size += borderSize;
	SetSize(size);
}

void MinWindow::RegisterComponent(WindowComponentBase& component)
{
	if (!component.isRegistered && component.pParent == this)
	{
		components.Add(&component);
		component.isRegistered = true;
	}
}

MSG MinWindow::RunMessageLoop()
{
	isInitialized = true;

	while (PollWindowMessages())
	{
		for (WindowComponentBase* component : components)
		{
			component->Update();
		}
	}

	DWORD execFlags = ES_CONTINUOUS;

	if (!canSysSleep)
		execFlags |= ES_SYSTEM_REQUIRED;

	if (!canDispSleep)
		execFlags |= ES_DISPLAY_REQUIRED;

	SetThreadExecutionState(execFlags);

	return wndMsg;
}

bool MinWindow::PollWindowMessages()
{
	// Process windows events
	if (PeekMessage(&wndMsg, nullptr, 0, 0, PM_REMOVE))
	{
		if (wndMsg.message == WM_QUIT)
			return false;

		TranslateMessage(&wndMsg);
		DispatchMessage(&wndMsg);
	}

	return true;
}

void MinWindow::EnableStyleFlags(WndStyle flags)
{
	WndStyle style = GetStyle();
	style.x |= flags.x;
	style.y |= flags.y;
	SetStyle(style);
}

void MinWindow::DisableStyleFlags(WndStyle flags)
{
	WndStyle style = GetStyle();
	style.x &= ~flags.x;
	style.y &= ~flags.y;
	SetStyle(style);
}

static MONITORINFOEXW GetMonInfo(HMONITOR mon)
{
	MONITORINFOEXW info;
	info.cbSize = sizeof(MONITORINFOEXW);

	WIN_CHECK_NZ_LAST(GetMonitorInfoW(mon, &info));
	return info;
}

void MinWindow::SetActiveMonitor(HMONITOR newMon)
{
	if (newMon == GetActiveMonitor())
		return;

	MONITORINFOEXW info = GetMonInfo(newMon);
	RECT rect = info.rcMonitor;
	ivec2 pos(rect.left, rect.top);
	const bool wasFullscreen = isFullscreen;

	if (wasFullscreen)
		SetFullScreen(false);

	SetPos(pos);

	if (wasFullscreen)
		SetFullScreen(true);
}

HMONITOR MinWindow::GetActiveMonitor() const { return MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST); }

WinMonConfig MinWindow::GetMonitorConfig(HMONITOR mon)
{
	MONITORINFOEXW info = GetMonInfo(mon);
	DEVMODEW mode = {};
	mode.dmSize = sizeof(DEVMODEW);

	WIN_CHECK_NZ_LAST(EnumDisplaySettingsExW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode, 0));

	WinMonConfig cfg = {};
	cfg.res = uivec2(mode.dmPelsWidth, mode.dmPelsHeight);
	cfg.pos = ivec2(mode.dmPosition.x, mode.dmPosition.y);
	cfg.refreshHz = mode.dmDisplayFrequency;
	cfg.bitsPerPixel = mode.dmBitsPerPel;

	return cfg;
}

WinMonConfig MinWindow::GetActiveMonitorConfig() const { return GetMonitorConfig(GetActiveMonitor()); }

ivec2 MinWindow::GetMonitorDPI() const
{
	HMONITOR mon = GetActiveMonitor();
	uivec2 dpi;
	WIN_CHECK_HR(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpi.x, &dpi.y));

	return ivec2((int)dpi.x, (int)dpi.y);
}

vec2 MinWindow::GetNormMonitorDPI() const
{
	ivec2 dpi = GetMonitorDPI();
	return vec2(dpi.x / 96.0f, dpi.y / 96.0f);
}

ivec2 MinWindow::GetMonitorPosition() const
{
	HMONITOR mon = GetActiveMonitor();
	MONITORINFOEXW info = GetMonInfo(mon);
	RECT rect = info.rcMonitor;

	return ivec2(rect.left, rect. top);
}

ivec2 MinWindow::GetMonitorResolution() const
{
	HMONITOR mon = GetActiveMonitor();
	MONITORINFOEXW info = GetMonInfo(mon);
	RECT rect = info.rcMonitor;

	return ivec2(rect.right - rect.left, rect.bottom - rect.top);
}

/// <summary>
/// Returns true if the window is in borderless fullscreen mode
/// </summary>
bool MinWindow::GetIsFullScreen() const { return isFullscreen; }

/// <summary>
/// Enable/disable borderless full screen
/// </summary>
void MinWindow::SetFullScreen(bool value)
{
	if (value != isFullscreen)
	{
		isFullscreen = value;

		if (isFullscreen)
		{
			lastPos = GetPos();
			lastSize = GetSize();

			SetStyle(WndStyle(WS_VISIBLE | WS_POPUP, 0));
			SetSize(GetMonitorResolution());
			SetPos(GetMonitorPosition());
		}
		else
		{
			SetStyle(initStyle);
			SetSize(lastSize);
			SetPos(lastPos);
		}
	}
}

bool MinWindow::GetIsMousedOver() const
{
	return isMousedOver;
}

ivec2 MinWindow::GetGlobalCursorPos() const
{
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	WIN_CHECK_NZ_LAST(GetCursorInfo(&info));

	return ivec2(info.ptScreenPos.x, info.ptScreenPos.y);
}

void MinWindow::SetIsCursorVisible(bool value)
{
	if (value == GetIsCursorVisible())
		return;

	if (!value)
		while(ShowCursor(false) >= 0);
	else
		while(ShowCursor(true) < 0);
}

bool MinWindow::GetIsCursorVisible() const
{
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	WIN_CHECK_NZ_LAST(GetCursorInfo(&info));

	return info.flags == CURSOR_SHOWING;
}

void MinWindow::SetCanDisplaySleep(bool value)
{
	canDispSleep = value;
}

bool MinWindow::GetCanDisplaySleep() const
{
	return canDispSleep;
}

void MinWindow::SetCanSystemSleep(bool value)
{
	canSysSleep = value;
}

bool MinWindow::GetCanSystemSleep() const
{
	return canSysSleep;
}

LRESULT MinWindow::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		// Update internal window state
		switch (msg)
		{
		case WM_CLOSE: // Window closed, normal exit
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			UpdateSize();
			break;
		case WM_ACTIVATE:
		case WM_SETFOCUS:
		case WM_MOUSEACTIVATE:
		case WM_NCMOUSEHOVER:
		case WM_NCMOUSEMOVE:
		case WM_MOUSEHOVER:
		case WM_MOUSEMOVE:

			if (!isMousedOver)
				TrackMouseEvent(&tme);

			isMousedOver = true;
			break;
		case WM_KILLFOCUS:
		case WM_MOUSELEAVE:
		case WM_NCMOUSELEAVE:
			isMousedOver = false;
			break;
		}

		// Non-client override
		if (GetIsNonClientCustom())
		{
			switch (msg)
			{
				case WM_NCCALCSIZE:
				case WM_NCPAINT:
					return 0;
				case WM_GETMINMAXINFO:
				{
					MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
					const HMONITOR mon = GetActiveMonitor();
					const MONITORINFOEXW info = GetMonInfo(mon);
					const RECT workArea = info.rcWork;
					
					// Set maximized size and track size
					pInfo->ptMaxSize.x = workArea.right - workArea.left;
					pInfo->ptMaxSize.y = workArea.bottom - workArea.top - 1; // Workaround for padding
					
					pInfo->ptMaxTrackSize.x = workArea.right - workArea.left;
					pInfo->ptMaxTrackSize.y = workArea.bottom - workArea.top;

					// Set maximized position
					pInfo->ptMaxPosition = {};

					return 0;
				}
				case WM_NCHITTEST:
				{
					POINTS pos = MAKEPOINTS(lParam);
					const ivec2 wndPos = GetPos();
					const ivec2 mndSize = GetSize();
					const ivec2 cursorPos(pos.x - wndPos.x, pos.y - wndPos.y);
					const ivec2 padding = paddingOverride;

					if (AllTrue(cursorPos > ivec2(0) && cursorPos < wndSize))
					{
						const ivec2 innerMax = (wndSize - padding);

						// Corners
						if (cursorPos.x < padding.x && cursorPos.y < padding.y)		return HTTOPLEFT;
						if (cursorPos.x > innerMax.x && cursorPos.y < padding.y)	return HTTOPRIGHT;
						if (cursorPos.x < padding.x && cursorPos.y > innerMax.y)	return HTBOTTOMLEFT;
						if (cursorPos.x > innerMax.x && cursorPos.y > innerMax.y)	return HTBOTTOMRIGHT;

						// Edges
						if (cursorPos.x < padding.x)	return HTLEFT;
						if (cursorPos.x > innerMax.x)	return HTRIGHT;
						if (cursorPos.y < padding.y)	return HTTOP;
						if (cursorPos.y > innerMax.y)	return HTBOTTOM;

						// Header
						if (isHeaderHovered)
							return HTCAPTION;

						return HTCLIENT;
					}
					else
						return HTNOWHERE;
				}
			}
		}

		// Pass messages through to components
		if (msg != WM_CLOSE && isInitialized)
		{
			for (WindowComponentBase* component : components)
			{
				// Allow components to intercept messages from later components
				if (!component->OnWndMessage(hWnd, msg, wParam, lParam))
					break;
			}
		}

	}
	catch (...)
	{ }

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void MinWindow::UpdateSize()
{
	RECT clientBox, wndBox;

	if (GetClientRect(hWnd, &clientBox) != 0)
		bodySize = ivec2(clientBox.right - clientBox.left, clientBox.bottom - clientBox.top);

	if (GetWindowRect(hWnd, &wndBox) != 0)
		wndSize = ivec2(wndBox.right - wndBox.left, wndBox.bottom - wndBox.top);

	if (isNcCustom)
		WV_ASSERT_MSG(bodySize == wndSize, "Client and Window size should be equal when non-client area is overridden.");
}

LRESULT CALLBACK MinWindow::HandleWindowSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		// Retrieve custom window pointer from create data
		const CREATESTRUCTW* const pData = reinterpret_cast<CREATESTRUCTW*>(lParam);
		MinWindow* wndPtr = static_cast<MinWindow*>(pData->lpCreateParams);

		// Add pointer to user data field
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wndPtr));

		// Switch to main message forwarding proceedure
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowMessageHandler));

		return wndPtr->OnWndMessage(hWnd, msg, wParam, lParam);
	}
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK MinWindow::WindowMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MinWindow* const wndPtr = reinterpret_cast<MinWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return wndPtr->OnWndMessage(hWnd, msg, wParam, lParam);
}