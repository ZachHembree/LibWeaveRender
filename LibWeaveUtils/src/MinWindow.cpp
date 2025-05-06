#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "WeaveUtils/WeaveWinException.hpp"
#include <ShellScalingApi.h>

#pragma comment(lib, "Shcore.lib")

using namespace Weave;

MinWindow::MinWindow() :
	ComponentManagerBase(),
	hInst(nullptr),
	hWnd(nullptr),
	wndMsg(MSG{}),
	bodySize(0),
	wndSize(0),
	isInitialized(false),
	isMousedOver(false),
	isFullscreen(false),
	lastPos(0),
	lastSize(0),
	canSysSleep(true),
	canDispSleep(true)
{ }

MinWindow::MinWindow(
	wstring_view name,
	ivec2 bodySize,
	const HINSTANCE hInst,
	const wchar_t* iconRes,
	WndStyle style,
	const WndStyleOverride* pStyleOverride
) :
	MinWindow()
{
	name = name;
	this->hInst = hInst;
	this->style = style;
	this->pStyleOverride = pStyleOverride;

	// Setup window descriptor
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC; // Own device context
	wc.lpfnWndProc = &HandleWindowSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = name.data();

	if (iconRes != nullptr)
	{
		wc.hIcon = (HICON)LoadImage(hInst, iconRes, IMAGE_ICON, 64, 64, 0);
		wc.hIconSm = (HICON)LoadImage(hInst, iconRes, IMAGE_ICON, 32, 32, 0);
	}
	else
	{
		wc.hIcon = nullptr;
		wc.hIconSm = nullptr;
	}

	// Register window class
	WIN_ASSERT_NZ_LAST(RegisterClassEx(&wc));
	
	RECT wr;
	wr.left = 50;
	wr.right = bodySize.x + wr.left;
	wr.top = 50;
	wr.bottom = bodySize.y + wr.top;

	// Resize body if not overriding non-client area
	if (pStyleOverride == nullptr)
		WIN_ASSERT_NZ_LAST(AdjustWindowRect(&wr, style.x, FALSE));

	// Create window instance
	hWnd = CreateWindowExW(
		style.y,
		name.data(),
		name.data(),
		style.x,
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

	// Setup mouse tracker
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = hWnd;
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;

	// Initial draw and show window
	RepaintWindow();
}

MinWindow::MinWindow(
	wstring_view name,
	ivec2 bodySize,
	const HINSTANCE hInst,
	WndStyle style,
	const wchar_t* iconRes
) :
	MinWindow(name, bodySize, hInst, iconRes, style, nullptr)
{ }

MinWindow::MinWindow(
	wstring_view name,
	ivec2 bodySize,
	const HINSTANCE hInst,
	const WndStyleOverride& styleOverride,
	const wchar_t* iconRes
) :
	MinWindow(name, bodySize, hInst, iconRes, g_DefaultWndStyle, &styleOverride)
{ }

MinWindow::~MinWindow()
{
	if (hWnd != nullptr)
	{
		isInitialized = false;
		UnregisterClass(name.data(), hInst);
		DestroyWindow(hWnd);
	}
}

MSG MinWindow::RunMessageLoop()
{
	isInitialized = true;

	while (PollWindowMessages())
	{
		for (WindowComponentBase* pComp : sortedComps)
			pComp->Update();
	}

	return wndMsg;
}

wstring_view MinWindow::GetWindowTitle() const
{
	size_t len = GetWindowTextLengthW(hWnd) + 1;
	title.resize(len, '\0');
	WIN_ASSERT_NZ_LAST(GetWindowTextW(hWnd, title.data(), (int)len));

	return title;
}

void MinWindow::SetWindowTitle(wstring_view text)
{
	WIN_CHECK_NZ_LAST(SetWindowTextW(hWnd, text.data()));
}

const wstring_view MinWindow::GetName() const { return name; }

HINSTANCE MinWindow::GetProcHandle() const { return hInst; }

HWND MinWindow::GetWndHandle() const { return hWnd; }

/*
	Styling, placement and sizing
*/

WndStyle MinWindow::GetStyle() const
{
	WndStyle style;

	style.x = (DWORD)GetWindowLongPtr(hWnd, GWL_STYLE);
	WIN_ASSERT_NZ_LAST(style.x);

	style.y = (DWORD)GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	WIN_ASSERT_NZ_LAST(style.y);

	return style;
}

bool MinWindow::GetIsStyleOverridden() const { return pStyleOverride != nullptr; }

void MinWindow::SetStyle(WndStyle style)
{
	WIN_CHECK_NZ_LAST(SetWindowLongPtr(hWnd, GWL_STYLE, style.x));

	if (style.y != 0L)
		WIN_CHECK_NZ_LAST(SetWindowLongPtr(hWnd, GWL_EXSTYLE, style.y));

	RepaintWindow();
}

void MinWindow::RepaintWindow()
{
	WIN_CHECK_NZ_LAST(SetWindowPos(
		hWnd,
		HWND_TOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
	WIN_CHECK_NZ_LAST(InvalidateRect(hWnd, 0, NULL));
	WIN_CHECK_NZ_LAST(ShowWindow(hWnd, SW_SHOW));
}

bool MinWindow::GetIsFullScreen() const { return isFullscreen; }

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
			SetStyle(style);
			SetSize(lastSize);
			SetPos(lastPos);
		}
	}
}

ivec2 MinWindow::GetSize() const { return wndSize; }

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

ivec2 MinWindow::GetBodySize() const { return bodySize; }

void MinWindow::SetBodySize(ivec2 size)
{
	ivec2 borderSize = wndSize - bodySize;
	size += borderSize;
	SetSize(size);
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

/*
	System utilities
*/

bool MinWindow::GetIsMousedOver() const { return isMousedOver; }

ivec2 MinWindow::GetGlobalCursorPos() const
{
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	WIN_CHECK_NZ_LAST(GetCursorInfo(&info));

	return ivec2(info.ptScreenPos.x, info.ptScreenPos.y);
}

bool MinWindow::GetIsCursorVisible() const
{
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	WIN_CHECK_NZ_LAST(GetCursorInfo(&info));

	return info.flags == CURSOR_SHOWING;
}

void MinWindow::SetIsCursorVisible(bool value)
{
	if (value == GetIsCursorVisible())
		return;

	if (!value)
		while (ShowCursor(false) >= 0);
	else
		while (ShowCursor(true) < 0);
}

bool MinWindow::GetCanDisplaySleep() const { return canDispSleep; }

void MinWindow::SetCanDisplaySleep(bool value) { canDispSleep = value; }

bool MinWindow::GetCanSystemSleep() const { return canSysSleep; }

void MinWindow::SetCanSystemSleep(bool value) { canSysSleep = value; }

/*
	Monitor utilities
*/

static MONITORINFOEXW GetMonInfo(HMONITOR mon)
{
	MONITORINFOEXW info;
	info.cbSize = sizeof(MONITORINFOEXW);

	WIN_CHECK_NZ_LAST(GetMonitorInfoW(mon, &info));
	return info;
}

HMONITOR MinWindow::GetActiveMonitor() const { return MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST); }

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

WndMonConfig MinWindow::GetActiveMonitorConfig() const { return GetMonitorConfig(GetActiveMonitor()); }

WndMonConfig MinWindow::GetMonitorConfig(HMONITOR mon)
{
	MONITORINFOEXW info = GetMonInfo(mon);
	DEVMODEW mode = {};
	mode.dmSize = sizeof(DEVMODEW);

	WIN_CHECK_NZ_LAST(EnumDisplaySettingsExW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode, 0));

	WndMonConfig cfg = {};
	cfg.res = uivec2(mode.dmPelsWidth, mode.dmPelsHeight);
	cfg.pos = ivec2(mode.dmPosition.x, mode.dmPosition.y);
	cfg.refreshHz = mode.dmDisplayFrequency;
	cfg.bitsPerPixel = mode.dmBitsPerPel;

	return cfg;
}

ivec2 MinWindow::GetMonitorDPI() const
{
	HMONITOR mon = GetActiveMonitor();

	if (mon == nullptr)
		return ivec2(96);

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

void MinWindow::Minimize() { PostMessageW(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0); }

void MinWindow::Maximize()
{
	if (IsZoomed(hWnd))
		WIN_CHECK_NZ_LAST(PostMessageW(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0));
	else
		WIN_CHECK_NZ_LAST(PostMessageW(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0));
}

void MinWindow::CloseWindow() { PostMessageW(hWnd, WM_CLOSE, 0, 0); }

/*
	Win32 message polling and callbacks
*/

bool MinWindow::PollWindowMessages()
{
	UpdateComponentIDs();

	// Process windows events
	if (PeekMessage(&wndMsg, nullptr, 0, 0, PM_REMOVE))
	{
		if (wndMsg.message == WM_QUIT)
			return false;

		TranslateMessage(&wndMsg);
		DispatchMessage(&wndMsg);
	}

	// Update thread execution state
	DWORD execFlags = ES_CONTINUOUS;

	if (!canSysSleep)
		execFlags |= ES_SYSTEM_REQUIRED;

	if (!canDispSleep)
		execFlags |= ES_DISPLAY_REQUIRED;

	SetThreadExecutionState(execFlags);

	return true;
}

LRESULT MinWindow::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	std::optional<slong> result;

#ifdef NDEBUG
	try
	{
#endif
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
		if (pStyleOverride != nullptr)
			result = TryHandleOverrideNC(hWnd, msg, wParam, lParam);

		// Pass messages through to components
		if (msg != WM_CLOSE && isInitialized)
		{
			for (WindowComponentBase* pComp : sortedComps)
			{
				// Allow earlier components to intercept messages from later components
				if (!pComp->OnWndMessage(hWnd, msg, wParam, lParam))
					break;
			}
		}
#ifdef NDEBUG
	}
	catch (const WeaveException& err)
	{
		WV_LOG_ERROR() << "An exception was thrown during Win32 message polling.\n"
			<< "[" << err.GetType() << "] " << err.GetDescription() << '\n';
	}
	catch (const std::exception& err)
	{
		WV_LOG_ERROR() << "An exception was thrown during Win32 message polling.\n"
			<< "Standard Exception: " << err.what() << '\n';
	}
	catch (...)
	{
		WV_LOG_ERROR() << "An exception was thrown during Win32 message polling.\n"
			<< "An unknown exception occurred." << '\n';
	}
#endif // ! WV_TRACE_EXCEPT

	return (result.has_value() ? result.value() : DefWindowProcW(hWnd, msg, wParam, lParam));
}

void MinWindow::UpdateSize()
{
	RECT clientBox, wndBox;

	if (GetClientRect(hWnd, &clientBox) != 0)
		bodySize = ivec2(clientBox.right - clientBox.left, clientBox.bottom - clientBox.top);

	if (GetWindowRect(hWnd, &wndBox) != 0)
		wndSize = ivec2(wndBox.right - wndBox.left, wndBox.bottom - wndBox.top);

	if (pStyleOverride != nullptr)
		WV_ASSERT_MSG(bodySize == wndSize, "Client and Window size should be equal when non-client area is overridden.");
}

std::optional<LRESULT> MinWindow::TryHandleOverrideNC(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
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
		case WM_NCLBUTTONDOWN:
		{
			switch (wParam)
			{
			case HTMINBUTTON:
				Minimize();
				return 0;
			case HTMAXBUTTON:
				Maximize();
				return 0;
			case HTCLOSE:
				CloseWindow();
				return 0;
			}
			break;
		}
		case WM_NCHITTEST:
		{
			POINTS pos = MAKEPOINTS(lParam);
			const ivec2 wndPos = GetPos();
			const ivec2 mndSize = GetSize();
			const ivec2 cursorPos(pos.x - wndPos.x, pos.y - wndPos.y);
			const ivec2 padding = pStyleOverride->GetPadding();

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

				switch (pStyleOverride->GetHitTestOverride())
				{
				case WndHitTestOverrides::Minimize:
					return HTMINBUTTON;
				case WndHitTestOverrides::Maximize:
					return HTMAXBUTTON;
				case WndHitTestOverrides::Close:
					return HTCLOSE;
				case WndHitTestOverrides::Caption:
					return HTCAPTION;
				}

				return HTCLIENT;
			}
			else
				return HTNOWHERE;
		}
	}
	
	return std::nullopt;
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

/*
	Style override description
*/

uivec2 WndStyleOverride::GetPadding() const { return padding; }

void WndStyleOverride::SetPadding(uivec2 padding) { this->padding = padding; }

WndHitTestOverrides WndStyleOverride::GetHitTestOverride() const { return htOverride; }

void WndStyleOverride::SetHitTestOverride(WndHitTestOverrides hitTest, bool isSet)
{
	if (htOverride == WndHitTestOverrides::None || hitTest == WndHitTestOverrides::None || hitTest == htOverride)
		htOverride = (isSet ? hitTest : WndHitTestOverrides::None);
}
