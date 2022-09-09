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

bool ImguiHandler::enableDemoWindow = false;

ImguiHandler::ImguiHandler() : pRenderComponent(nullptr) { }

ImguiHandler::ImguiHandler(MinWindow& window, Renderer& renderer) :
	WindowComponentBase(window)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window.GetWndHandle());
	pRenderComponent = new ImguiRenderComponent(renderer);
	UpdateUI();
}

ImguiHandler::~ImguiHandler()
{
	delete pRenderComponent;
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiHandler::UpdateUI()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	vec2 scale = GetWindow().GetNormMonitorDPI();
	float scaleDelta = (float)(scale.y / (double)io.FontGlobalScale);

	style.ScaleAllSizes(scaleDelta);
	io.FontGlobalScale = scale.y;

	pRenderComponent->enableDemoWindow = enableDemoWindow;
}

bool ImguiHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGuiIO& io = ImGui::GetIO();

		switch (msg)
		{
		case WM_ACTIVATE:
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			UpdateUI();
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
			return !(io.WantCaptureKeyboard || io.WantCaptureMouse);
		}
	}

	return true;
}