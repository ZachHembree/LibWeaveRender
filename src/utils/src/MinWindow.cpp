#include "MinWindow.hpp"
#include "WindowComponentBase.hpp"
#include "resource.h"

using namespace glm;
using namespace Replica;

// Has title bar | Has minimize button | Has window menu on its title bar
const long wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

MinWindow* MinWindow::pLastInit;

MinWindow::MinWindow(const HINSTANCE hInst, const ivec2 size) :
	components(),
	wndMsg(MSG{}),
	size(size),
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
	wc.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0);
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = g_Name;
	wc.hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0);

	// Register window class
	const ATOM classID = RegisterClassEx(&wc);

	if (classID == 0)
		throw REP_EXCEPT_LAST();

	// Get window rect to set body to the size given, not including border
	RECT wr;
	wr.left = 100;
	wr.right = size.x + wr.left;
	wr.top = 100;
	wr.bottom = size.y + wr.top;

	// Check if window sizing failed
	if (!AdjustWindowRect(&wr, wndStyle, FALSE))
		throw REP_EXCEPT_LAST();

	pLastInit = this;

	// Create window instance
	hWnd = CreateWindowW(
		g_Name,
		g_Name,
		wndStyle,
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
}

MinWindow::MinWindow(MinWindow&& other) noexcept :
	hInst(other.hInst),
	hWnd(other.hWnd),
	wndMsg(other.wndMsg),
	size(0)
{
	other.hInst = nullptr;
	other.hWnd = nullptr;
}

MinWindow& MinWindow::operator=(MinWindow&& rhs) noexcept
{
	hInst = rhs.hInst;
	hWnd = rhs.hWnd;
	wndMsg = rhs.wndMsg;

	rhs.hInst = nullptr;
	rhs.hWnd = nullptr;

	return *this;
}

MinWindow::~MinWindow()
{
	if (hWnd != nullptr)
	{
		UnregisterClass(GetName(), hInst);
		DestroyWindow(hWnd);
	}
}

const wchar_t* MinWindow::GetName() const noexcept
{
	return g_Name;
}

HINSTANCE MinWindow::GetProcHandle() const noexcept
{
	return hInst;
}

HWND MinWindow::GetWndHandle() const noexcept
{
	return hWnd;
}

void MinWindow::RegisterComponent(WindowComponentBase* component)
{
	if (component != nullptr && !component->isRegistered && component->parent == this)
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

LRESULT MinWindow::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE: // Window closed, normal exit
		PostQuitMessage(0);
		break;
	}

	for (auto component : components)
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