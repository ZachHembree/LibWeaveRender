#include "ReplicaWin32.hpp"
#include <ShellScalingApi.h>

using namespace glm;
using namespace Replica;

MinWindow* MinWindow::pLastInit;

MinWindow::MinWindow(
	wstring_view initName, 
	ivec2 initSize, 
	WndStyle initStyle, 
	const HINSTANCE hInst, 
	const wchar_t* iconRes
) :
	name(initName),
	bodySize(initSize),
	wndMsg(MSG{}),
	hInst(hInst),
	hWnd(nullptr),
	isFullscreen(false),
	isInitialized(false),
	isMousedOver(false),
	canSysSleep(true),
	canDispSleep(true),
	lastPos(0),
	lastSize(0),
	initStyle(initStyle)
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
	wr.right = bodySize.x + wr.left;
	wr.top = 50;
	wr.bottom = bodySize.y + wr.top;

	// Resize body
	WIN_ASSERT_NZ_LAST(AdjustWindowRect(&wr, initStyle.x, FALSE));
	pLastInit = this;

	// Create window instance
	hWnd = CreateWindowEx(
		initStyle.y,
		initName.data(),
		initName.data(),
		initStyle.x,
		// Starting position
		CW_USEDEFAULT, CW_USEDEFAULT,
		// Starting size
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr, nullptr,
		hInst,
		this
	);

	// Check if window creation failed
	if (hWnd == NULL)
		throw WIN_THROW_LAST();

	// Make the window visible
	ShowWindow(hWnd, SW_SHOW);

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

wstring MinWindow::GetWindowTitle() const
{
	size_t len = GetWindowTextLengthW(hWnd);
	wstring title(len, '\0');
	WIN_ASSERT_NZ_LAST(GetWindowTextW(hWnd, title.data(), (int)len));

	return title;
}

void MinWindow::SetWindowTitle(wstring_view text)
{
	WIN_ASSERT_NZ_LAST(SetWindowTextW(hWnd, text.data()));
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
	WIN_ASSERT_NZ_LAST(SetWindowLongPtr(hWnd, GWL_STYLE, style.x));

	if (style.y != 0L)
		WIN_ASSERT_NZ_LAST(SetWindowLongPtr(hWnd, GWL_EXSTYLE, style.y));

	// Update to reflect changes
	WIN_ASSERT_NZ_LAST(SetWindowPos(
		hWnd,
		HWND_TOPMOST,
		0, 0,
		0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

ivec2 MinWindow::GetPos() const
{
	RECT rect;
	WIN_ASSERT_NZ_LAST(GetWindowRect(hWnd, &rect));
	return ivec2(rect.left, rect.top);
}

void MinWindow::SetPos(ivec2 pos)
{
	WIN_ASSERT_NZ_LAST(SetWindowPos(
		hWnd,
		0,
		pos.x, pos.y,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
	));
}

ivec2 MinWindow::GetSize() const
{
	return wndSize;
}

void MinWindow::SetSize(ivec2 size)
{
	WIN_ASSERT_NZ_LAST(SetWindowPos(
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
		components.push_back(&component);
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

ivec2 MinWindow::GetMonitorDPI() const
{
	HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	uivec2 dpi;
	WIN_THROW_HR(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpi.x, &dpi.y));

	return ivec2((int)dpi.x, (int)dpi.y);
}

vec2 MinWindow::GetNormMonitorDPI() const
{
	ivec2 dpi = GetMonitorDPI();
	return vec2(dpi.x / 96.0f, dpi.y / 96.0f);
}

ivec2 MinWindow::GetMonitorPosition() const
{
	HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	WIN_ASSERT_NZ_LAST(GetMonitorInfo(mon, &info));

	RECT rect = info.rcMonitor;
	return ivec2(rect.left, rect. top);
}

ivec2 MinWindow::GetMonitorResolution() const
{
	HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);

	WIN_ASSERT_NZ_LAST(GetMonitorInfo(mon, &info));

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

			DisableStyleFlags(initStyle);
			SetSize(GetMonitorResolution());
			SetPos(GetMonitorPosition());
		}
		else
		{
			EnableStyleFlags(initStyle);
			SetSize(lastSize);
			SetPos(lastPos);
		}
	}
}

bool MinWindow::GetIsMousedOver() const
{
	return isMousedOver;
}

void MinWindow::SetIsCursorVisible(bool value)
{
	if (value == GetIsCursorVisible())
		return;

	if (!value)
	{
		while(ShowCursor(false) >= 0);
	}
	else
	{
		while(ShowCursor(true) < 0);
	}
}

bool MinWindow::GetIsCursorVisible() const
{
	CURSORINFO info;
	info.cbSize = sizeof(CURSORINFO);
	WIN_ASSERT_NZ_LAST(GetCursorInfo(&info));

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
		switch (msg)
		{
		case WM_CLOSE: // Window closed, normal exit
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			OnResize();
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

void MinWindow::OnResize()
{
	RECT clientBox, wndBox;

	GetClientRect(hWnd, &clientBox);
	GetWindowRect(hWnd, &wndBox);

	bodySize = ivec2(clientBox.right - clientBox.left, clientBox.bottom - clientBox.top);
	wndSize = ivec2(wndBox.right - wndBox.left, wndBox.bottom - wndBox.top);
}

LRESULT CALLBACK MinWindow::HandleWindowSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_CREATE)
	{
		// **BROKEN**
		// Retrieve custom window pointer from create data
		const CREATESTRUCTW* const pData = reinterpret_cast<CREATESTRUCTW*>(lParam);
		MinWindow* wndPtr = pLastInit;//static_cast<MinWindow*>(pData->lpCreateParams);
		pLastInit = nullptr;

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