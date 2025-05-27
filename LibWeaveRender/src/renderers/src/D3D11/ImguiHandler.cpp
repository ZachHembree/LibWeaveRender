#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/ImGuiHandler.hpp"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandlerEx(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, ImGuiIO& io);

using namespace Weave;
using namespace Weave::D3D11;

ImGuiHandler* ImGuiHandler::s_pInstance;
bool ImGuiHandler::enableDemoWindow = false;

ImGuiHandler::ImGuiHandler(MinWindow& parent, Renderer& renderer) :
	WindowComponentBase(parent, WndCompPriorities::BeforeInput),
	pRenderer(&renderer),
	pRenderHook(&renderer.CreateComponent<RenderHook>(RenderOrders::DrawUI + 1)),
	isInitialized(true),
	frameNum(0)
{ 
	Device& dev = renderer.GetDevice();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GetWindow().GetWndHandle());	
	ImGui_ImplDX11_Init(dev.Get(), dev.GetImmediateContext());
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	pRenderHook->SetCallback(RenderStages::Setup, [this](CtxImm& ctx) { this->Setup(ctx); });
	pRenderHook->SetCallback(RenderStages::Draw, [this](CtxImm& ctx) { this->Draw(ctx); });
}

ImGuiHandler::~ImGuiHandler()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	ImPlot::DestroyContext();
	s_pInstance = nullptr;
}

void ImGuiHandler::Setup(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		// Pass through buffered Win32 events
		const auto& messages = win32Buf.GetMessages();

		for (const auto& msg : messages)
			ImGui_ImplWin32_WndProcHandlerEx(msg.hWnd, msg.msg, msg.wParam, msg.lParam, ImGui::GetIO());

		// Check input capture
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		wantsKeyboard = io.WantCaptureKeyboard;
		wantsMouse = io.WantCaptureMouse;

		// Update DPI scaling
		vec2 scale = GetWindow().GetNormMonitorDPI();
		float scaleDelta = (float)(scale.y / (double)io.FontGlobalScale);
		style.ScaleAllSizes(scaleDelta);
		io.FontGlobalScale = scale.y;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		if (enableDemoWindow)
			ImGui::ShowDemoWindow();
	}
}

void ImGuiHandler::Draw(CtxImm& ctx)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
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

string& ImGuiHandler::GetTmpString() 
{
	const ulong currentFrame = pRenderer->GetFrameNumber();

	if (frameNum != currentFrame)
		ResetStrPool();

	return activeText.EmplaceBack(stringPool.Get()); 
}

void ImGuiHandler::ResetStrPool()
{
	// Reset pool
	for (slong i = activeText.GetLength() - 1; i >= 0; i--)
	{
		activeText[i].clear();
		stringPool.Return(std::move(activeText[i]));
	}

	activeText.Clear();
}

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

bool ImGuiHandler::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		// Record messages to be played back on the render thread
		win32Buf.AddMessage(hWnd, msg, wParam, lParam);

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
			if (wantsMouse)
				isInterrupting = true;
			[[fallthrough]];
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			if (wantsKeyboard)
				isInterrupting = true;
		}

		if (isInterrupting)
			return false;
	}

	return true;
}