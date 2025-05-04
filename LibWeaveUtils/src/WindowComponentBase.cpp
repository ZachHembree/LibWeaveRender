#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"

using namespace Weave;

uint WindowComponentBase::GetPriority() const { return priority; }

WindowComponentBase::WindowComponentBase() :
	pParent(nullptr),
	id(uint(-1)),
	priority(uint(-1))
{ }

WindowComponentBase::WindowComponentBase(MinWindow& window, uint priority) :
	pParent(&window),
	id(uint(-1)),
	priority(priority)
{
	window.RegisterComponent(*this);
}

WindowComponentBase::WindowComponentBase(WindowComponentBase&& other) noexcept :
	pParent(nullptr),
	id(uint(-1))
{
	if (GetIsRegistered())
		other.pParent->MoveComponent(*this, std::move(other));
}

WindowComponentBase& WindowComponentBase::operator=(WindowComponentBase&& other) noexcept
{
	if (GetIsRegistered())
		pParent->UnregisterComponent(*this);

	if (other.GetIsRegistered())
		other.pParent->MoveComponent(*this, std::move(other));

	return *this;
}

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