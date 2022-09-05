#include "D3D11/RenderComponent.hpp"
#include "D3D11/Renderer.hpp"

using namespace Replica::D3D11;

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
