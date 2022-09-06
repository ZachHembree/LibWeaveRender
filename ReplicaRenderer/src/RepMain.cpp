#include "MinWindow.hpp"
#include "Input.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "D3D11/DebugScene.hpp"
#include "resource.h"
#include <string_view>

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;
using namespace Microsoft::WRL;

// Has title bar | Has minimize button | Has window menu on its title bar | Can maximize | Can resize
const long wndStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_SIZEBOX;

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCMdLine, int nCmdShow)
{
	try
	{
		MSG lastMsg;

		{
			const wchar_t* iconFile = MAKEINTRESOURCE(IDI_ICON1);

			// Initialize Windows runtime
			Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
			THROW_FAILED(initialize);

			MinWindow repWindow(g_Name, ivec2(1280, 800), WndStyle(wndStyle, 0L), hInst, iconFile);
			InputComponent input(repWindow);

			Renderer renderer(repWindow);
			ImguiHandler imgui(repWindow, renderer);
			DebugScene debugScene(renderer, input);

			/* Fullscreen borderless window
			ivec2 mres = repWindow.GetMonitorResolution();
			repWindow.SetStyleBorderless();
			repWindow.SetSize(mres);
			repWindow.SetPos(ivec2(0));
			*/

			// Start main loop
			lastMsg = repWindow.RunMessageLoop();
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