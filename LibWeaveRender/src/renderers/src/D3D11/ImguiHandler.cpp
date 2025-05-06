#include "pch.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImGuiHandler.hpp"
#include "D3D11/ImGuiRenderComponent.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Weave;
using namespace Weave::D3D11;

ImGuiHandler* ImGuiHandler::s_pInstance;
bool ImGuiHandler::enableDemoWindow = false;

ImGuiHandler::ImGuiHandler(MinWindow& parent, Renderer& renderer) :
	WindowComponentBase(parent, 0),
	pRenderer(&renderer),
	isInitialized(true)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetWindow().GetWndHandle());
	pRenderer->CreateComponent(pRenderComponent);
	
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

ImGuiHandler::~ImGuiHandler()
{
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiHandler::GetIsInitialized() { return s_pInstance != nullptr && s_pInstance->isInitialized; }

void ImGuiHandler::Init(Renderer& renderer)
{
	if (!GetIsInitialized())
	{
		MinWindow& wnd = renderer.GetWindow();
		wnd.CreateComponent(s_pInstance, renderer);
	}
}

void ImGuiHandler::Reset() 
{ 
	if (GetIsInitialized())
	{
		s_pInstance->GetWindow().UnregisterComponent(*s_pInstance);
		s_pInstance = nullptr;
	}
}

string& ImGuiHandler::GetTmpString() { return activeText.EmplaceBack(stringPool.Get()); }

string_view ImGuiHandler::GetTmpNarrowStr(wstring_view str)
{
	WV_ASSERT(s_pInstance != nullptr);
	string& buf = s_pInstance->GetTmpString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf;
}

char* ImGuiHandler::GetTmpNarrowCStr(wstring_view str)
{
	WV_ASSERT(s_pInstance != nullptr);
	string& buf = s_pInstance->GetTmpString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf.data();
}

void ImGuiHandler::Update()
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		// Update DPI scaling
		vec2 scale = GetWindow().GetNormMonitorDPI();
		float scaleDelta = (float)(scale.y / (double)io.FontGlobalScale);
		style.ScaleAllSizes(scaleDelta);
		io.FontGlobalScale = scale.y;

		pRenderComponent->enableDemoWindow = enableDemoWindow;

		// Manually update display size and cursor position
		MinWindow& wnd = pRenderer->GetWindow();
		const ivec2 cursorPos = wnd.GetGlobalCursorPos() - wnd.GetBodyPos();
		const ivec2 vpSize = (pRenderer->GetWindowRenderMode() == WindowRenderModes::ExclusiveFS) ?
			wnd.GetMonitorResolution() : (ivec2)pRenderer->GetOutputResolution();
		const float aspectRatio = (float)vpSize.y / vpSize.x;
		const vec2 normPos = (1.0f / vpSize.y) * vec2(cursorPos.x * aspectRatio, cursorPos.y);

		const uivec2 newSize = pRenderer->GetOutputResolution();
		pRenderComponent->SetMousePos(normPos * vec2(newSize));
		pRenderComponent->SetDispSize(newSize);

		// Reset pool
		for (slong i = activeText.GetLength() - 1; i >= 0; i--)
		{
			activeText[i].clear();
			stringPool.Return(std::move(activeText[i]));
		}

		activeText.Clear();
	}
}

bool ImGuiHandler::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

		// Intercept input messages when requested
		const ImGuiIO& io = ImGui::GetIO();
		bool isInterrupting = false;

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
			if (io.WantCaptureMouse)
				isInterrupting = true;
			[[fallthrough]];
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			if (io.WantCaptureKeyboard)
				isInterrupting = true;
		}

		if (isInterrupting)
			return false;
	}

	return true;
}