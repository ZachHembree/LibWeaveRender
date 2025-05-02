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

ImguiRenderComponent::ImguiRenderComponent(Renderer& renderer) :
	RenderComponentBase(renderer),
	enableDemoWindow(false),
	mousePos(0),
	dispSize(0)
{
	Device& dev = renderer.GetDevice();
	ImGui_ImplDX11_Init(dev.Get(), dev.GetImmediateContext());
}

ImguiRenderComponent::~ImguiRenderComponent()
{ 
	ImGui_ImplDX11_Shutdown();
}

string& ImguiRenderComponent::GetTempString() { return activeText.EmplaceBack(stringPool.Get()); }

vec2 ImguiRenderComponent::GetMousePos() const { return mousePos; }

uivec2 ImguiRenderComponent::GetDispSize() const { return dispSize; }

void ImguiRenderComponent::SetMousePos(vec2 mousePos) { this->mousePos = mousePos; }

void ImguiRenderComponent::SetDispSize(uivec2 dispSize) { this->dispSize = dispSize; }

void ImguiRenderComponent::Setup(CtxImm& ctx)
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

		// Reset pool
		for (string& str : activeText)
		{
			str.clear();
			stringPool.Return(std::move(str));
		}

		activeText.Clear();

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
