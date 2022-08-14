#include "Input.hpp"
#include "MinWindow.hpp"

using namespace glm;
using namespace Replica;

void InputComponent::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	}
}

ivec2 InputComponent::GetMousePos()
{
	auto state = mouse.GetState();
	return ivec2(state.x, state.y);
}

vec2 InputComponent::GetNormMousePos()
{
	ivec2 screenSize = parent->GetSize();
	auto state = mouse.GetState();
	float aspectRatio = (float)screenSize.y / screenSize.x;
	vec2 pos(state.x * aspectRatio, -state.y);

	return (1.0f / screenSize.y) * pos;
}
