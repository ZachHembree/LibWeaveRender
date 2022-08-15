#pragma once
#include "RepLeanWin.h"
#include "RepMain.h"
#include "RepWinException.hpp"
#include "UniqueObj.hpp"

namespace Replica
{
	class MinWindow;

	/// <summary>
	/// Base class for window components
	/// </summary>
	class WindowComponentBase : protected UniqueObjBase
	{
	friend MinWindow;

	public:
		MinWindow* const parent;

		/// <summary>
		/// Called after each message poll in the window class
		/// </summary>
		virtual void Update() { }

		/// <summary>
		/// Invoked on WndProc callback
		/// </summary>
		virtual void OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { }

		/// <summary>
		/// Returns true if the component has been registered to a window
		/// </summary>
		bool GetIsRegistered();

	protected:

		WindowComponentBase(MinWindow* window);

	private:
		bool isRegistered;
	};
}