#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "D3D11/ImguiRenderComponent.hpp"
#include "InputHandler.hpp"

#include "WeaveRender/Imgui.hpp"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <math.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Weave;
using namespace Weave::D3D11;

std::unique_ptr<ImGuiHandler> pInstance;

bool ImGuiHandler::enableDemoWindow = false;

ImGuiHandler::ImGuiHandler(Renderer& renderer) :
	WindowComponentBase(renderer.GetWindow()),
	pRenderer(&renderer)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetWindow().GetWndHandle());
	pRenderComponent = new ImguiRenderComponent(renderer);	
	InputHandler::Init(GetWindow());

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

ImGuiHandler::~ImGuiHandler()
{
	delete pRenderComponent;
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiHandler::Init(Renderer& renderer)
{
	if (pInstance.get() == nullptr)
		pInstance.reset(new ImGuiHandler(renderer));
}

void ImGuiHandler::Reset() { pInstance.reset(); }

ImGuiHandler& ImGuiHandler::GetInstance() { D3D_ASSERT_MSG(pInstance.get(), "ImGuiHandler is null"); return *pInstance; }

string& ImGuiHandler::GetTempString() { return pRenderComponent->GetTempString(); }

string_view ImGuiHandler::GetTmpNarrowStr(wstring_view str)
{
	string& buf = GetInstance().GetTempString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf;
}

char* ImGuiHandler::GetTmpNarrowCStr(wstring_view str)
{
	string& buf = GetInstance().GetTempString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf.data();
}

void ImGuiHandler::UpdateUI()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	vec2 scale = GetWindow().GetNormMonitorDPI();
	float scaleDelta = (float)(scale.y / (double)io.FontGlobalScale);
	
	style.ScaleAllSizes(scaleDelta);
	io.FontGlobalScale = scale.y;

	pRenderComponent->enableDemoWindow = enableDemoWindow;

	// Manually update UI size and cursor position
	InputHandler::SetIsEnabled(true);
	const uivec2 newSize = pRenderer->GetOutputResolution();
	const vec2 normPos = InputHandler::GetNormMousePos();

	// Disable input when ImGui wants to capture	
	InputHandler::SetIsEnabled(!(io.WantCaptureKeyboard || io.WantCaptureMouse));

	pRenderComponent->SetDispSize(newSize);
	pRenderComponent->SetMousePos(normPos * vec2(newSize));
}

bool ImGuiHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

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
		}
	}

	return true;
}