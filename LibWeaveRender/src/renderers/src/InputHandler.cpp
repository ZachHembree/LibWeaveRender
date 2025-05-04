#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "../src/renderers/include/InputHandler.hpp"

using namespace Weave;
using namespace DirectX;

InputHandler InputHandler::s_Handler;
bool InputHandler::s_IsEnabled = true;

InputHandler::InputHandler() : 
	lastMousePos(0),
	isInitialized(false),
	currentMousePresses(MouseKey::None),
	lastMousePresses(MouseKey::None)
{ }

InputHandler::InputHandler(MinWindow& window) :
	WindowComponentBase(window, 10),
	lastMousePos(0),
	isInitialized(true),
	currentMousePresses(MouseKey::None),
	lastMousePresses(MouseKey::None),
	keyboard(new Keyboard()),
	mouse(new Mouse())
{ }

InputHandler::InputHandler(InputHandler&& other) noexcept :
	WindowComponentBase(std::move(other)),
	kbTracker(std::move(other.kbTracker)),
	currentMousePresses(other.currentMousePresses),
	lastMousePresses(other.lastMousePresses),
	keyboard(std::move(other.keyboard)),
	mouse(std::move(other.mouse)),
	lastMousePos(other.lastMousePos),
	isInitialized(other.isInitialized)
{ 
	other.isInitialized = false;
}

InputHandler& InputHandler::operator=(InputHandler&& other) noexcept
{
	WindowComponentBase::operator=(std::move(other));

	kbTracker = std::move(other.kbTracker);
	currentMousePresses = other.currentMousePresses;
	lastMousePresses = other.lastMousePresses;
	keyboard = std::move(other.keyboard);
	mouse = std::move(other.mouse);
	lastMousePos = other.lastMousePos;
	isInitialized = other.isInitialized;

	other.isInitialized = false;
	return *this;
}

InputHandler::~InputHandler() = default;

void InputHandler::Init(MinWindow& wnd)
{
	if (!s_Handler.isInitialized)
	{
		s_Handler = InputHandler(wnd);
		s_IsEnabled = true;
		s_Handler.lastMousePos = ivec2(0);
		s_Handler.currentMousePresses = MouseKey::None;
		s_Handler.lastMousePresses = MouseKey::None;
	}
}

bool InputHandler::GetIsEnabled() { return s_IsEnabled; }

void InputHandler::SetIsEnabled(bool value) { s_IsEnabled = value; }

MouseKey InputHandler::GetLastPressedMouseKeys()
{
	return s_IsEnabled ? s_Handler.lastMousePresses : MouseKey::None;
}

MouseKey InputHandler::GetPresssedMouseKeys()
{		
	return s_IsEnabled ? s_Handler.currentMousePresses : MouseKey::None;
}

bool InputHandler::GetIsNewKeyPressed(MouseKey key)
{
	return s_IsEnabled && GetIsKeyPressed(key) && !GetWasKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(MouseKey key)
{
	return s_IsEnabled && (uint)(key & GetLastPressedMouseKeys());
}

bool InputHandler::GetIsKeyPressed(MouseKey key)
{
	return s_IsEnabled && (uint)(key & GetPresssedMouseKeys());
}

bool InputHandler::GetIsNewKeyPressed(KbKey key)
{
	return s_IsEnabled && s_Handler.kbTracker.IsKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(KbKey key)
{
	return s_IsEnabled && s_Handler.kbTracker.IsKeyReleased(key);
}

bool InputHandler::GetIsKeyPressed(KbKey key)
{
	return s_IsEnabled && s_Handler.kbTracker.lastState.IsKeyDown(key);
}

ivec2 InputHandler::GetMousePos()
{
	auto state = Mouse::Get().GetState();
	s_Handler.lastMousePos = s_IsEnabled ? ivec2(state.x, state.y) : s_Handler.lastMousePos;
	return s_Handler.lastMousePos;
}

vec2 InputHandler::GetNormMousePos()
{
	ivec2 vpSize = s_Handler.GetWindow().GetBodySize(),
		pos = GetMousePos();
	float aspectRatio = (float)vpSize.y / vpSize.x;

	return (1.0f / vpSize.y) * vec2(pos.x * aspectRatio, pos.y);
}

bool InputHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		Mouse::ProcessMessage(msg, wParam, lParam);
		break;
	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(msg, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_SYSKEYDOWN:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		break;
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		Keyboard::Get().Reset();
		break;
	}	

	kbTracker.Update(Keyboard::Get().GetState());
	MouseState state = Mouse::Get().GetState();
	lastMousePresses = currentMousePresses;
	currentMousePresses = MouseKey::None;

	if (state.leftButton)
		currentMousePresses |= MouseKey::LeftButton;
	else if (state.middleButton)
		currentMousePresses |= MouseKey::MiddleButton;
	else if (state.rightButton)
		currentMousePresses |= MouseKey::RightButton;
	else if (state.xButton2)
		currentMousePresses |= MouseKey::xButton1;
	else if (state.xButton2)
		currentMousePresses |= MouseKey::xButton2;

	return true;
}

