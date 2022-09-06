#include "MinWindow.hpp"
#include "WindowComponentBase.hpp"

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
	size(initSize),
	lastStyle(initStyle),
	wndMsg(MSG{}),
	hInst(hInst),
	hWnd(nullptr)
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
	const ATOM classID = RegisterClassEx(&wc);

	if (classID == NULL)
		throw REP_EXCEPT_LAST();

	// Get window rect to set body to the size given, not including border
	RECT wr;
	wr.left = 100;
	wr.right = size.x + wr.left;
	wr.top = 100;
	wr.bottom = size.y + wr.top;

	// Check if window sizing failed
	if (!AdjustWindowRect(&wr, initStyle.x, FALSE))
		throw REP_EXCEPT_LAST();

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
		throw REP_EXCEPT_LAST();

	// Make the window visible
	ShowWindow(hWnd, SW_SHOW);
	initStyle = GetStyle();
}

MinWindow::MinWindow(MinWindow&& other) noexcept :
	name(other.name),
	lastStyle(other.lastStyle),
	hInst(other.hInst),
	hWnd(other.hWnd),
	wndMsg(other.wndMsg),
	size(other.size)
{
	memset(&other, 0, sizeof(MinWindow));
}

MinWindow& MinWindow::operator=(MinWindow&& rhs) noexcept
{
	memcpy(this, &rhs, sizeof(MinWindow));
	rhs.hInst = nullptr;
	rhs.hWnd = nullptr;

	return *this;
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
	WIN_ASSERT_NZ_LAST(GetWindowTextW(hWnd, title.data(), len));

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
	return size;
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

void MinWindow::RegisterComponent(WindowComponentBase* component)
{
	if (component != nullptr && !component->isRegistered && component->pParent == this)
	{
		components.push_back(component);
		component->isRegistered = true;
	}
}

MSG MinWindow::RunMessageLoop()
{
	while (PollWindowMessages())
	{
		for (int i = 0; i < components.GetLength(); i++)
		{
			components[i]->Update();
		}
	}

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

void MinWindow::SetStyleBorderless()
{
	WndStyle style = GetStyle();
	lastStyle = style;
	style.x &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetStyle(style);
}

void MinWindow::ResetStyle()
{
	SetStyle(lastStyle);
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

LRESULT MinWindow::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: // Window closed, normal exit
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		size = ivec2(width, height);
		break;
	}

	for (WindowComponentBase* component : components)
	{
		component->OnWndMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
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