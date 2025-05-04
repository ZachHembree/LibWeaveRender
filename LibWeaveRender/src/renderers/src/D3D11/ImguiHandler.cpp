#include "pch.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImGuiHandler.hpp"
#include "D3D11/ImGuiRenderComponent.hpp"

#include <imgui.h>
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Weave;
using namespace Weave::D3D11;

ImGuiHandler ImGuiHandler::s_Instance;
bool ImGuiHandler::enableDemoWindow = false;

ImGuiHandler::ImGuiHandler() :
	pRenderer(nullptr),
	pRenderComponent(nullptr),
	isInitialized(false)
{ }

ImGuiHandler::ImGuiHandler(Renderer& renderer) :
	WindowComponentBase(renderer.GetWindow(), 0),
	pRenderer(&renderer),
	isInitialized(true)
{ 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetWindow().GetWndHandle());
	pRenderComponent.reset(new ImGuiRenderComponent(renderer));

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

ImGuiHandler::ImGuiHandler(ImGuiHandler&& other) noexcept :
	WindowComponentBase(std::move(other)),
	pRenderer(other.pRenderer),
	isInitialized(other.isInitialized),
	pRenderComponent(std::move(other.pRenderComponent))
{
	other.isInitialized = false;
}

ImGuiHandler& ImGuiHandler::operator=(ImGuiHandler&& other) noexcept
{
	WindowComponentBase::operator=(std::move(other));

	pRenderer = other.pRenderer;
	isInitialized = other.isInitialized;
	pRenderComponent = std::move(other.pRenderComponent);
	other.isInitialized= false;
	return *this;
}

ImGuiHandler::~ImGuiHandler()
{	
	if (isInitialized)
	{
		pRenderComponent.reset();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

bool ImGuiHandler::GetIsInitialized() { return s_Instance.isInitialized; }

void ImGuiHandler::Init(Renderer& renderer)
{
	if (!s_Instance.isInitialized)
		s_Instance = ImGuiHandler(renderer);
}

void ImGuiHandler::Reset() { s_Instance = ImGuiHandler(); }

string& ImGuiHandler::GetTmpString() { return activeText.EmplaceBack(stringPool.Get()); }

string_view ImGuiHandler::GetTmpNarrowStr(wstring_view str)
{
	string& buf = s_Instance.GetTmpString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf;
}

char* ImGuiHandler::GetTmpNarrowCStr(wstring_view str)
{
	string& buf = s_Instance.GetTmpString();
	GetMultiByteString_UTF16LE_TO_UTF8(str, buf);
	return buf.data();
}

bool ImGuiHandler::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{ 
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

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
		ivec2 wndSize = pRenderer->GetWindow().GetBodySize(),
			pos = wnd.GetGlobalCursorPos() - wnd.GetBodyPos();
		float aspectRatio = (float)wndSize.y / wndSize.x;
		const vec2 normPos = (1.0f / wndSize.y) * vec2(pos.x * aspectRatio, pos.y);

		const uivec2 newSize = pRenderer->GetOutputResolution();
		pRenderComponent->SetMousePos(normPos * vec2(newSize)); //
		pRenderComponent->SetDispSize(newSize);

		// Reset pool
		for (string& str : activeText)
		{
			str.clear();
			stringPool.Return(std::move(str));
		}

		activeText.Clear();

		// Intercept input messages when requested
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