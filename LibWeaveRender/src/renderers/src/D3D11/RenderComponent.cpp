#include "pch.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/RenderComponent.hpp"

using namespace Weave;
using namespace Weave::D3D11;

RenderComponentBase::RenderComponentBase(Renderer& renderer, uint priority) :
	Renderer::CompBaseT(renderer, priority)
{ }

Renderer& RenderComponentBase::GetRenderer() { return *pParent; }

Device& RenderComponentBase::GetDevice() { return pParent->GetDevice(); }

MinWindow& RenderComponentBase::GetWindow() { return pParent->GetWindow(); }

const Renderer& RenderComponentBase::GetRenderer() const { return *pParent; }

const Device& RenderComponentBase::GetDevice() const { return pParent->GetDevice(); }

const MinWindow& RenderComponentBase::GetWindow() const { return pParent->GetWindow(); }
