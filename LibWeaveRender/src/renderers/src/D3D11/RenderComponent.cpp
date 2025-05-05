#include "pch.hpp"
#include "WeaveRender/D3D11.hpp"

using namespace Weave;
using namespace Weave::D3D11;

RenderComponentBase::RenderComponentBase(Renderer& renderer, uint priority) :
	pRenderer(&renderer), priority(priority), id(uint(-1))
{ }

Renderer& RenderComponentBase::GetRenderer() { return *pRenderer; }

Device& RenderComponentBase::GetDevice() { return pRenderer->GetDevice(); }

MinWindow& RenderComponentBase::GetWindow() { return pRenderer->GetWindow(); }

const Renderer& RenderComponentBase::GetRenderer() const { return *pRenderer; }

const Device& RenderComponentBase::GetDevice() const { return pRenderer->GetDevice(); }

const MinWindow& RenderComponentBase::GetWindow() const { return pRenderer->GetWindow(); }

uint RenderComponentBase::GetPriority() const { return priority; }

bool RenderComponentBase::GetIsRegistered(Renderer* pRenderer) const
{ 
	if (pRenderer == nullptr)
		return id != uint(-1);
	else 
		return pRenderer == this->pRenderer && id != uint(-1);
}
