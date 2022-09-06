#include "MinWindow.hpp"
#include "Input.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "DebugScene.hpp"
#include "resource.h"
#include <string_view>
#include <ShellScalingApi.h>

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
			WIN_THROW_HR(initialize);

			// This application will handle its own scaling
			WIN_THROW_HR(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));

			MinWindow repWindow(g_Name, ivec2(1280, 800), WndStyle(wndStyle, 0L), hInst, iconFile);
			Renderer renderer(repWindow);
			ImguiHandler imgui(repWindow, renderer);
			InputComponent input(repWindow);
			DebugScene debugScene(renderer, input);

			ivec2 dpi = repWindow.GetMonitorDPI(),
				monRes = repWindow.GetMonitorResolution(),
				scaledRes = (96 * monRes) / dpi;
			vec2 fDpi = repWindow.GetNormMonitorDPI();

			// Fullscreen borderless window
			/*ivec2 mres = repWindow.GetMonitorResolution();
			repWindow.DisableStyleFlags(WndStyle(wndStyle, 0));
			repWindow.SetBodySize(mres);
			repWindow.SetPos(ivec2(0));*/

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