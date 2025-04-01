#include "pch.hpp"
#include "ReplicaRender/D3D11.hpp"

using namespace Replica;
using namespace Replica::D3D11;

Renderer& RenderComponentBase::GetRenderer() { return *pRenderer; }

Device& RenderComponentBase::GetDevice() { return pRenderer->GetDevice(); }

MinWindow& RenderComponentBase::GetWindow() { return GetRenderer().GetWindow(); }

bool RenderComponentBase::GetIsRegistered() { return isRegistered; }

void RenderComponentBase::Register(Renderer& renderer)
{
	if (!isRegistered)
	{
		renderer.RegisterComponent(*this);
	}
}

void RenderComponentBase::Unregister()
{
	if (isRegistered)
	{
		pRenderer->UnregisterComponent(*this);
	}
}

RenderComponentBase::RenderComponentBase() : pRenderer(nullptr), isRegistered(false)
{ }

RenderComponentBase::RenderComponentBase(Renderer & renderer) : pRenderer(nullptr), isRegistered(false)
{
	Register(renderer);
}

RenderComponentBase::~RenderComponentBase()
{
	Unregister();
}
