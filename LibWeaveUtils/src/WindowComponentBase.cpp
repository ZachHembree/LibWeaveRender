#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"

using namespace Weave;

WindowComponentBase::WindowComponentBase() :
	pParent(nullptr),
	isRegistered(false)
{ }

WindowComponentBase::WindowComponentBase(MinWindow& window) :
	pParent(&window),
	isRegistered(false)
{
	window.RegisterComponent(*this);
}

/// <summary>
/// Returns pointer to parent window
/// </summary>
MinWindow& WindowComponentBase::GetWindow() const { return *pParent; }

bool WindowComponentBase::GetIsRegistered()
{
	return isRegistered && pParent != nullptr;
}