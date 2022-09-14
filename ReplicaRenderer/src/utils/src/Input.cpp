#include "ReplicaMain.hpp"

using namespace glm;
using namespace Replica;
using namespace DirectX;

MouseKey InputComponent::GetLastPressedMouseKeys() const
{
	return lastMousePresses;
}

MouseKey InputComponent::GetPresssedMouseKeys() const
{		
	return currentMousePresses;
}

bool InputComponent::GetIsNewKeyPressed(MouseKey key) const
{
	return GetIsKeyPressed(key) && !GetWasKeyPressed(key);
}

bool InputComponent::GetWasKeyPressed(MouseKey key) const
{
	return (uint)(key & GetLastPressedMouseKeys());
}

bool InputComponent::GetIsKeyPressed(MouseKey key) const
{
	return (uint)(key & GetPresssedMouseKeys());
}

bool InputComponent::GetIsKeyPressed(KbKey key) const
{
	return keyboard.GetState().IsKeyDown(key);
}

ivec2 InputComponent::GetMousePos() const
{
	auto state = mouse.GetState();
	return ivec2(state.x, state.y);
}

vec2 InputComponent::GetNormMousePos() const
{
	ivec2 vpSize = GetWindow().GetSize(), pos = GetMousePos();
	float aspectRatio = (float)vpSize.y / vpSize.x;

	return (1.0f / vpSize.y) * vec2(pos.x * aspectRatio, pos.y);
}

InputComponent::InputComponent(MinWindow& window) :
	WindowComponentBase(window)
{ }

bool InputComponent::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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

	auto state = mouse.GetState();
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
