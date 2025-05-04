#include "pch.hpp"
#include "D3D11/ImGuiRenderComponent.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Device.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace Weave::D3D11;

ImGuiRenderComponent::ImGuiRenderComponent() = default;

ImGuiRenderComponent::ImGuiRenderComponent(Renderer& renderer) :
	RenderComponentBase(renderer),
	enableDemoWindow(false),
	mousePos(0),
	dispSize(0)
{
	Device& dev = renderer.GetDevice();
	ImGui_ImplDX11_Init(dev.Get(), dev.GetImmediateContext());
}

ImGuiRenderComponent::~ImGuiRenderComponent()
{ 
	ImGui_ImplDX11_Shutdown();
}

vec2 ImGuiRenderComponent::GetMousePos() const { return mousePos; }

uivec2 ImGuiRenderComponent::GetDispSize() const { return dispSize; }

void ImGuiRenderComponent::SetMousePos(vec2 mousePos) { this->mousePos = mousePos; }

void ImGuiRenderComponent::SetDispSize(uivec2 dispSize) { this->dispSize = dispSize; }

void ImGuiRenderComponent::Setup(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		if (dispSize != uivec2(0))
			io.DisplaySize = ImVec2((float)dispSize.x, (float)dispSize.y);
			
		if (mousePos != vec2(0))
			io.AddMousePosEvent(mousePos.x, mousePos.y);

		ImGui::NewFrame();	

		if (enableDemoWindow)
			ImGui::ShowDemoWindow();
	}
}

void ImGuiRenderComponent::DrawLate(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
