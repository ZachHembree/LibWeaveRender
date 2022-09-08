#include "resource.h"
#include <string_view>
#include <ShellScalingApi.h>
#include "MinWindow.hpp"
#include "Input.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/ImguiHandler.hpp"
#include "DebugImageViewer.hpp"
#include "DebugScene.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;
using namespace Microsoft::WRL;

int CALLBACK WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE, _In_ LPSTR lpCMdLine, _In_ int nCmdShow)
{
	try
	{
		MSG lastMsg;
		const wchar_t* iconFile = MAKEINTRESOURCE(IDI_ICON1);

		// Initialize Windows runtime
		Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
		WIN_THROW_HR(initialize);

		// This application will handle its own scaling
		WIN_THROW_HR(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE));

		MinWindow repWindow(g_Name, ivec2(1280, 800), WndStyle(g_DefaultWndStyle, 0L), hInst, iconFile);
		Renderer renderer(repWindow);
		ImguiHandler imgui(repWindow, renderer);
		InputComponent input(repWindow);

		//DebugImageViewer viewer(renderer);
		DebugScene debugScene(renderer, input);

		// Fullscreen borderless window
		ivec2 mres = repWindow.GetMonitorResolution();
		repWindow.DisableStyleFlags(WndStyle(g_DefaultWndStyle, 0));
		repWindow.SetBodySize(mres);
		repWindow.SetPos(ivec2(0));

		// Restore window
		repWindow.EnableStyleFlags(WndStyle(g_DefaultWndStyle, 0));
		repWindow.SetBodySize(vec2(1280, 800));

		// Start main loop
		lastMsg = repWindow.RunMessageLoop();

		return (int)lastMsg.wParam;
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