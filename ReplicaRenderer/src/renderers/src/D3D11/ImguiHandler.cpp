#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "D3D11/ImguiRenderComponent.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <math.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Replica::D3D11;

ImguiHandler::ImguiHandler() : pRenderComponent(nullptr) { }

ImguiHandler::ImguiHandler(MinWindow& window, Renderer& renderer) :
	WindowComponentBase(window)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window.GetWndHandle());
	pRenderComponent = new ImguiRenderComponent(renderer);

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();
	vec2 scale = GetParent().GetNormMonitorDPI();

	style.ScaleAllSizes(scale.y);
	io.FontGlobalScale = scale.y;
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
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	vec2 scale = GetParent().GetNormMonitorDPI();
	float scaleDelta = (float)(scale.y / (double)io.FontGlobalScale);

	style.ScaleAllSizes(scaleDelta);
	io.FontGlobalScale = scale.y;
}