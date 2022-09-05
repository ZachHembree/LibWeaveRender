#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "D3D11/ImguiRenderComponent.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Replica::D3D11;

ImguiHandler::ImguiHandler() : pRenderComponent(nullptr) { }

ImguiHandler::ImguiHandler(MinWindow& window, Renderer& renderer) :
	WindowComponentBase(window)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window.GetWndHandle());
	pRenderComponent = new ImguiRenderComponent(renderer);
}

ImguiHandler::~ImguiHandler()
{
	delete pRenderComponent;
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}