#include "D3D11/ImguiRenderComponent.hpp"
#include "D3D11/Renderer.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace Replica::D3D11;

bool ImguiRenderComponent::enableDemoWindow = false;

ImguiRenderComponent::ImguiRenderComponent()
{ }

ImguiRenderComponent::ImguiRenderComponent(Renderer& renderer) :
	RenderComponentBase(renderer)
{
	Device& dev = renderer.GetDevice();
	ImGui_ImplDX11_Init(&dev.Get(), &dev.GetContext().Get());
}

ImguiRenderComponent::~ImguiRenderComponent()
{ 
	ImGui_ImplDX11_Shutdown();
}

void ImguiRenderComponent::Update(Context& ctx)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// IMGUI Test
	if (enableDemoWindow)
		ImGui::ShowDemoWindow();
}

void ImguiRenderComponent::DrawLate(Context& ctx)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}