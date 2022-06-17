#include "RepMain.h"
#include "RepLeanWin.h"
#include "MinWindow.hpp"
#include "RendererD3D11.hpp"

using namespace glm;

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCMdLine, int nCmdShow)
{
	try
	{
		MinWindow repWindow(hInst, ivec2(640, 480));
		RendererD3D11 renderer(&repWindow);

		// Start main loop
		const MSG lastMsg = repWindow.RunMessageLoop();

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