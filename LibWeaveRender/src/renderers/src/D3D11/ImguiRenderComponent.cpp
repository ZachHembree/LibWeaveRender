#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ImguiRenderComponent.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Device.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace Weave::D3D11;

ImguiRenderComponent::ImguiRenderComponent() : enableDemoWindow(false)
{ }

ImguiRenderComponent::ImguiRenderComponent(Renderer& renderer, InputHandler& input) :
	RenderComponentBase(renderer),
	enableDemoWindow(false),
	pInput(&input)
{
	Device& dev = renderer.GetDevice();
	ImGui_ImplDX11_Init(dev.Get(), dev.GetImmediateContext());
}

ImguiRenderComponent::~ImguiRenderComponent()
{ 
	ImGui_ImplDX11_Shutdown();
}

void ImguiRenderComponent::Setup(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGuiIO& io = ImGui::GetIO();
		const vec2 newSize = GetRenderer().GetOutputResolution();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		
		const vec2 newPos = pInput->GetNormMousePos() * newSize;
		io.DisplaySize = ImVec2(newSize.x, newSize.y);
		io.AddMousePosEvent(newPos.x, newPos.y);

		ImGui::NewFrame();	

		if (enableDemoWindow)
			ImGui::ShowDemoWindow();
	}
}

void ImguiRenderComponent::DrawLate(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}