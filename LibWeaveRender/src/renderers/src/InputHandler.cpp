#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/Renderer.hpp"
#include "../src/renderers/include/InputHandler.hpp"

using namespace Weave;
using namespace Weave::D3D11;
using namespace DirectX;

InputHandler* InputHandler::s_pHandler;
std::atomic<bool> InputHandler::s_IsEnabled = true;

InputHandler::InputHandler(MinWindow& parent, Renderer& rnd) :
	WindowComponentBase(parent, 1),
	pRenderHook(&rnd.CreateComponent<RenderHook>(1)),
	lastMousePos(0),
	keyboard(new Keyboard()),
	mouse(new Mouse())
{ 
	pRenderHook->SetCallback(RenderStages::Setup, [this](CtxImm& ctx) { Setup(ctx); });
}

InputHandler::~InputHandler() = default;

void InputHandler::Init(Renderer& rnd)
{
	if (!GetIsInitialized())
	{
		rnd.GetWindow().CreateComponent(s_pHandler, rnd);
		s_IsEnabled = true;
		s_pHandler->lastMousePos = ivec2(0);
	}
}

bool InputHandler::GetIsInitialized() { return s_pHandler != nullptr; }

bool InputHandler::GetIsEnabled() { return s_IsEnabled; }

void InputHandler::SetIsEnabled(bool value) { s_IsEnabled = value; }

bool InputHandler::GetIsNewKeyPressed(MouseKey key)
{
	return s_pHandler->msTracker.GetIsNewKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(MouseKey key)
{
	return s_pHandler->msTracker.GetWasKeyPressed(key);
}

bool InputHandler::GetIsKeyPressed(MouseKey key)
{
	return s_pHandler->msTracker.GetIsKeyPressed(key);
}

bool InputHandler::GetIsNewKeyPressed(KbKey key)
{
	return s_pHandler->kbTracker.IsKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(KbKey key)
{
	return s_pHandler->kbTracker.IsKeyReleased(key);
}

bool InputHandler::GetIsKeyPressed(KbKey key)
{
	return s_pHandler->kbTracker.lastState.IsKeyDown(key);
}

ivec2 InputHandler::GetMousePos() { return s_pHandler->msTracker.GetMousePos(); }

vec2 InputHandler::GetNormMousePos()
{
	WV_ASSERT(s_pHandler != nullptr);
	ivec2 vpSize = s_pHandler->GetWindow().GetBodySize(),
		pos = GetMousePos();
	float aspectRatio = (float)vpSize.y / vpSize.x;

	return (1.0f / vpSize.y) * vec2(pos.x * aspectRatio, pos.y);
}

void InputHandler::GetInputState(KbState& kbState, MouseState& msState)
{
	kbState = s_pHandler->keyboard->GetState();
	msState = s_pHandler->mouse->GetState();
}

void InputHandler::Setup(CtxImm& ctx)
{
	const auto& messages = msgBuf.GetMessages();

	for (const auto& msg : messages)
	{
		switch (msg.msg)
		{
		case WM_ACTIVATEAPP:
			Keyboard::ProcessMessage(msg.msg, msg.wParam, msg.lParam);
			Mouse::ProcessMessage(msg.msg, msg.wParam, msg.lParam);
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
			Mouse::ProcessMessage(msg.msg, msg.wParam, msg.lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			Keyboard::ProcessMessage(msg.msg, msg.wParam, msg.lParam);
			break;
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			Keyboard::Get().Reset();
			break;
		}
	}

	kbTracker.Update(keyboard->GetState());
	msTracker.Update(mouse->GetState());
}

bool InputHandler::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
{
	msgBuf.AddMessage(hWnd, msg, wParam, lParam);
	return true;
}

ivec2 InputHandler::MouseTracker::GetMousePos() const { return mousePos; }

ivec2 InputHandler::MouseTracker::GetLastMousePos() const { return lastMousePos; }

ivec2 InputHandler::MouseTracker::GetMousePosDelta() const { return mousePos - lastMousePos; }

bool InputHandler::MouseTracker::GetWasKeyPressed(MouseKey key) const { return (uint)(key & lastMousePresses); }

bool InputHandler::MouseTracker::GetIsKeyPressed(MouseKey key) const { return (uint)(key & currentMousePresses); }

bool InputHandler::MouseTracker::GetIsNewKeyPressed(MouseKey key) const { return GetIsKeyPressed(key) && !GetWasKeyPressed(key); }

void InputHandler::MouseTracker::Update(const MouseState& state)
{
	lastMousePos = mousePos;
	mousePos = ivec2(state.x, state.y);

	lastMousePresses = currentMousePresses;
	currentMousePresses = MouseKey::None;

	if (state.leftButton)
		currentMousePresses |= MouseKey::LeftButton;
	if (state.middleButton)
		currentMousePresses |= MouseKey::MiddleButton;
	if (state.rightButton)
		currentMousePresses |= MouseKey::RightButton;
	if (state.xButton2)
		currentMousePresses |= MouseKey::xButton1;
	if (state.xButton2)
		currentMousePresses |= MouseKey::xButton2;
}

void InputHandler::MouseTracker::Reset()
{
	currentMousePresses = MouseKey::None;
	lastMousePresses = MouseKey::None;
	lastMousePos = ivec2(0);
	mousePos = ivec2(0);
}
