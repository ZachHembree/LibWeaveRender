#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "../src/renderers/include/InputHandler.hpp"

using namespace Weave;
using namespace DirectX;

InputHandler* InputHandler::s_pHandler;
bool InputHandler::s_IsEnabled = true;

InputHandler::InputHandler(MinWindow& parent) :
	WindowComponentBase(parent, 1),
	lastMousePos(0),
	isInitialized(true),
	currentMousePresses(MouseKey::None),
	lastMousePresses(MouseKey::None),
	keyboard(new Keyboard()),
	mouse(new Mouse())
{ }

InputHandler::~InputHandler() = default;

void InputHandler::Init(MinWindow& wnd)
{
	if (!GetIsInitialized())
	{
		wnd.RegisterNewComponent(s_pHandler);
		s_IsEnabled = true;
		s_pHandler->lastMousePos = ivec2(0);
		s_pHandler->currentMousePresses = MouseKey::None;
		s_pHandler->lastMousePresses = MouseKey::None;
	}
}

bool InputHandler::GetIsInitialized() { return s_pHandler != nullptr && s_pHandler->isInitialized; }

bool InputHandler::GetIsEnabled() { return s_IsEnabled; }

void InputHandler::SetIsEnabled(bool value) { s_IsEnabled = value; }

MouseKey InputHandler::GetLastPressedMouseKeys()
{
	return s_IsEnabled ? s_pHandler->lastMousePresses : MouseKey::None;
}

MouseKey InputHandler::GetPresssedMouseKeys()
{		
	return s_IsEnabled ? s_pHandler->currentMousePresses : MouseKey::None;
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
	return s_IsEnabled && s_pHandler->kbTracker.IsKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(KbKey key)
{
	return s_IsEnabled && s_pHandler->kbTracker.IsKeyReleased(key);
}

bool InputHandler::GetIsKeyPressed(KbKey key)
{
	return s_IsEnabled && s_pHandler->kbTracker.lastState.IsKeyDown(key);
}

ivec2 InputHandler::GetMousePos()
{
	auto state = Mouse::Get().GetState();
	s_pHandler->lastMousePos = s_IsEnabled ? ivec2(state.x, state.y) : s_pHandler->lastMousePos;
	return s_pHandler->lastMousePos;
}

vec2 InputHandler::GetNormMousePos()
{
	WV_ASSERT(s_pHandler != nullptr);
	ivec2 vpSize = s_pHandler->GetWindow().GetBodySize(),
		pos = GetMousePos();
	float aspectRatio = (float)vpSize.y / vpSize.x;

	return (1.0f / vpSize.y) * vec2(pos.x * aspectRatio, pos.y);
}

void InputHandler::Update()
{
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

	return true;
}

