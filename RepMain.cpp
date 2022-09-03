#include "RepMain.h"
#include "RepLeanWin.h"
#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;
using namespace Microsoft::WRL;

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCMdLine, int nCmdShow)
{
	try
	{
		MSG lastMsg;

		{
			// Initialize Windows runtime
			Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
			THROW_FAILED(initialize);

			MinWindow repWindow(hInst, ivec2(1280, 800));

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(repWindow.GetWndHandle());

			Renderer renderer(&repWindow);

			// Start main loop
			lastMsg = repWindow.RunMessageLoop();

			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		return (int)lastMsg.wParam;
	}
	catch (const RepWinException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const RepException& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr,"No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}