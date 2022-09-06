#include "WindowComponentBase.hpp"
#include "MinWindow.hpp"

using namespace Replica;

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
MinWindow& WindowComponentBase::GetParent() const { return *pParent; }

bool WindowComponentBase::GetIsRegistered()
{
	return isRegistered && pParent != nullptr;
}