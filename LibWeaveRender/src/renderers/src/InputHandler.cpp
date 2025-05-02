#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "../src/renderers/include/InputHandler.hpp"

using namespace Weave;
using namespace DirectX;

std::unique_ptr<InputHandler> s_pHandler;

InputHandler::InputHandler(MinWindow& window) :
	WindowComponentBase(window)
{ }

InputHandler::~InputHandler() = default;

void InputHandler::Init(MinWindow& wnd)
{
	if (s_pHandler.get() == nullptr)
	{
		s_pHandler.reset(new InputHandler(wnd));
		s_pHandler->isEnabled = true;
		s_pHandler->lastMousePos = ivec2(0);
		s_pHandler->currentMousePresses = MouseKey::None;
		s_pHandler->lastMousePresses = MouseKey::None;
	}
}

InputHandler& InputHandler::GetInstance()
{
	return *s_pHandler;
}

bool InputHandler::GetIsEnabled() { return s_pHandler->isEnabled; }

void InputHandler::SetIsEnabled(bool value) { s_pHandler->isEnabled = value; }

MouseKey InputHandler::GetLastPressedMouseKeys()
{
	return s_pHandler->isEnabled ? s_pHandler->lastMousePresses : MouseKey::None;
}

MouseKey InputHandler::GetPresssedMouseKeys()
{		
	return s_pHandler->isEnabled ? s_pHandler->currentMousePresses : MouseKey::None;
}

bool InputHandler::GetIsNewKeyPressed(MouseKey key)
{
	return s_pHandler->isEnabled && GetIsKeyPressed(key) && !GetWasKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(MouseKey key)
{
	return s_pHandler->isEnabled && (uint)(key & GetLastPressedMouseKeys());
}

bool InputHandler::GetIsKeyPressed(MouseKey key)
{
	return s_pHandler->isEnabled && (uint)(key & GetPresssedMouseKeys());
}

bool InputHandler::GetIsNewKeyPressed(KbKey key)
{
	return s_pHandler->isEnabled && s_pHandler->kbTracker.IsKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(KbKey key)
{
	return s_pHandler->isEnabled && s_pHandler->kbTracker.IsKeyReleased(key);
}

bool InputHandler::GetIsKeyPressed(KbKey key)
{
	return s_pHandler->isEnabled && s_pHandler->kbTracker.lastState.IsKeyDown(key);
}

ivec2 InputHandler::GetMousePos()
{
	auto state = Mouse::Get().GetState();
	s_pHandler->lastMousePos = s_pHandler->isEnabled ? ivec2(state.x, state.y) : s_pHandler->lastMousePos;
	return s_pHandler->lastMousePos;
}

vec2 InputHandler::GetNormMousePos()
{
	ivec2 vpSize = s_pHandler->GetWindow().GetBodySize(),
		pos = GetMousePos();
	float aspectRatio = (float)vpSize.y / vpSize.x;

	return (1.0f / vpSize.y) * vec2(pos.x * aspectRatio, pos.y);
}

bool InputHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	kbTracker.Update(keyboard.GetState());

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		keyboard.ProcessMessage(msg, wParam, lParam);
		mouse.ProcessMessage(msg, wParam, lParam);
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
		mouse.ProcessMessage(msg, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyboard.ProcessMessage(msg, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		keyboard.ProcessMessage(msg, wParam, lParam);
		break;
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		keyboard.Reset();
		break;
	}	

	MouseState state = mouse.GetState();
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

