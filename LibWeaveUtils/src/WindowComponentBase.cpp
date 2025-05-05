#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"

using namespace Weave;

uint WindowComponentBase::GetPriority() const { return priority; }

WindowComponentBase::WindowComponentBase(uint priority) :
	pParent(nullptr),
	id(uint(-1)),
	priority(priority)
{ }

MinWindow& WindowComponentBase::GetWindow() const { return *pParent; }

void WindowComponentBase::Update() {}

bool WindowComponentBase::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return true; }

bool WindowComponentBase::GetIsRegistered(MinWindow* pParent)
{
	if (pParent == nullptr)
		return id != uint(-1) && this->pParent != nullptr;
	else
		return id != uint(-1) && this->pParent == pParent;
}