#include "RepMain.h"
#include "RepLeanWin.h"
#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"

using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;
using namespace Microsoft::WRL;

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCMdLine, int nCmdShow)
{
	try
	{
		// Initialize Windows runtime
		Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
		THROW_FAILED(initialize);

		MinWindow repWindow(hInst, ivec2(640, 480));
		Renderer renderer(&repWindow);

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