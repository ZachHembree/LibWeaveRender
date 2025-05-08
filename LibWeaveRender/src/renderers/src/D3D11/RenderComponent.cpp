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

RenderHook::RenderHook(Renderer& renderer, uint priority) :
	RenderComponentBase(renderer, priority)
{}

const RenderHook::CallbackT& RenderHook::GetCallback(RenderStages stage) const { return callbacks[(byte)stage]; }

void RenderHook::SetCallback(RenderStages stage, CallbackT&& pFunc) { callbacks[(byte)stage] = std::move(pFunc); }

void RenderHook::Setup(CtxImm& ctx)
{
	if (GetCallback(RenderStages::Setup) != nullptr)
		GetCallback(RenderStages::Setup)(ctx);
}

void RenderHook::Draw(CtxImm& ctx)
{
	if (GetCallback(RenderStages::Draw) != nullptr)
		GetCallback(RenderStages::Draw)(ctx);
}

void RenderHook::AfterDraw(CtxImm& ctx)
{
	if (GetCallback(RenderStages::AfterDraw) != nullptr)
		GetCallback(RenderStages::AfterDraw)(ctx);
}
