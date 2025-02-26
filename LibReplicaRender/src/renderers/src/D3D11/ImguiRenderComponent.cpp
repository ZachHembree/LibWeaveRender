#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/ImguiRenderComponent.hpp"
#include "D3D11/Renderer.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace Replica::D3D11;

ImguiRenderComponent::ImguiRenderComponent() : enableDemoWindow(false)
{ }

ImguiRenderComponent::ImguiRenderComponent(Renderer& renderer) :
	RenderComponentBase(renderer),
	enableDemoWindow(false)
{
	Device& dev = renderer.GetDevice();
	ImGui_ImplDX11_Init(&dev.Get(), &dev.GetContext().Get());
}

ImguiRenderComponent::~ImguiRenderComponent()
{ 
	ImGui_ImplDX11_Shutdown();
}

void ImguiRenderComponent::Setup(Context& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (enableDemoWindow)
			ImGui::ShowDemoWindow();
	}
}

void ImguiRenderComponent::DrawLate(Context& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}