#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "../src/renderers/include/InputHandler.hpp"

using namespace Weave;
using namespace DirectX;

InputHandler* InputHandler::s_pHandler;
std::atomic<bool> InputHandler::s_IsEnabled = true;

const InputHandler::KbTracker& InputHandler::ReadKeyboard() const { return kbTrackers[readIndex]; }

const InputHandler::MouseTracker& InputHandler::ReadMouse() const { return msTrackers[readIndex]; }

InputHandler::InputHandler(MinWindow& parent) :
	WindowComponentBase(parent, 1),
	lastMousePos(0),
	keyboard(new Keyboard()),
	mouse(new Mouse()),
	writeIndex(0),
	readIndex(g_TrackHistSize - 1)
{ }

InputHandler::~InputHandler() = default;

void InputHandler::Init(MinWindow& wnd)
{
	if (!GetIsInitialized())
	{
		wnd.CreateComponent(s_pHandler);
		s_IsEnabled = true;
		s_pHandler->lastMousePos = ivec2(0);
	}
}

bool InputHandler::GetIsInitialized() { return s_pHandler != nullptr; }

bool InputHandler::GetIsEnabled() { return s_IsEnabled; }

void InputHandler::SetIsEnabled(bool value) { s_IsEnabled = value; }

bool InputHandler::GetIsNewKeyPressed(MouseKey key)
{
	return s_pHandler->ReadMouse().GetIsNewKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(MouseKey key)
{
	return s_pHandler->ReadMouse().GetWasKeyPressed(key);
}

bool InputHandler::GetIsKeyPressed(MouseKey key)
{
	return s_pHandler->ReadMouse().GetIsKeyPressed(key);
}

bool InputHandler::GetIsNewKeyPressed(KbKey key)
{
	return s_pHandler->ReadKeyboard().IsKeyPressed(key);
}

bool InputHandler::GetWasKeyPressed(KbKey key)
{
	return s_pHandler->ReadKeyboard().IsKeyReleased(key);
}

bool InputHandler::GetIsKeyPressed(KbKey key)
{
	return s_pHandler->ReadKeyboard().lastState.IsKeyDown(key);
}

ivec2 InputHandler::GetMousePos() { return s_pHandler->ReadMouse().GetMousePos(); }

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
	const uint idx = s_pHandler->readIndex;
	kbState = s_pHandler->kbStates[idx];
	msState = s_pHandler->msStates[idx];
}

void InputHandler::Update()
{
	// Trackers need to see the states they missed and the current state
	for (uint offset = 0; offset < g_TrackHistSize; offset++)
	{
		const uint index = ((writeIndex + 1) + offset) % g_TrackHistSize;

		if (index != writeIndex)
		{
			kbTrackers[writeIndex].Update(kbStates[index]);
			msTrackers[writeIndex].Update(msStates[index]);
		}
	}

	kbStates[writeIndex] = Keyboard::Get().GetState();
	msStates[writeIndex] = Mouse::Get().GetState();

	kbTrackers[writeIndex].Update(kbStates[writeIndex]);
	msTrackers[writeIndex].Update(msStates[writeIndex]);

	// Make next state visible
	readIndex = (readIndex + 1) % g_TrackHistSize;
	writeIndex = (writeIndex + 1) % g_TrackHistSize;
}

bool InputHandler::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
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
