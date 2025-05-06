#include "pch.hpp"
#include "WeaveUtils/WindowComponentBase.hpp"
#include "WeaveUtils/MinWindow.hpp"

using namespace Weave;

WindowComponentBase::WindowComponentBase(MinWindow& parent, uint priority) :
	MinWindow::CompBaseT(parent, priority)
{ }

MinWindow& WindowComponentBase::GetWindow() const { return *pParent; }

void WindowComponentBase::Update() {}

bool WindowComponentBase::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return true; }
