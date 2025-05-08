#include "pch.hpp"
#include "WeaveUtils/WindowComponentBase.hpp"
#include "WeaveUtils/MinWindow.hpp"

using namespace Weave;

WindowComponentBase::WindowComponentBase(MinWindow& parent, uint priority) :
	MinWindow::CompBaseT(parent, priority)
{ }

MinWindow& WindowComponentBase::GetWindow() const { return *pParent; }

void WindowComponentBase::Update() {}

bool WindowComponentBase::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) { return true; }

WindowHook::WindowHook(MinWindow& wnd, uint priority) :
	WindowComponentBase(wnd, priority)
{ }

const WindowHook::UpdateCallbackT& WindowHook::GetUpdateCallback() const { return updateFunc; }

void WindowHook::SetUpdateCallback(UpdateCallbackT&& func) { updateFunc = std::move(func); }

const WindowHook::MsgCallbackT& WindowHook::GetMsgCallback() const { return msgFunc; }

void WindowHook::SetMsgCallback(MsgCallbackT&& func) { msgFunc = func; }

void WindowHook::Update()
{
	if (updateFunc != nullptr)
		updateFunc();
}

bool WindowHook::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
{
	if (msgFunc != nullptr)
		return msgFunc(hWnd, msg, wParam, lParam);

	return true;
}
