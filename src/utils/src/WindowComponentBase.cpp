#include "WindowComponentBase.hpp"

using namespace Replica;

WindowComponentBase::WindowComponentBase(MinWindow* window) :
	parent(window),
	isRegistered(false)
{
	if (window != nullptr)
		window->RegisterComponent(this);
	else
		throw std::exception("Parent window cannot be null");
}

bool WindowComponentBase::GetIsRegistered()
{
	return isRegistered && parent != nullptr;
}