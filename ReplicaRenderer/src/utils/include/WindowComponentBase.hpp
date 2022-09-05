#pragma once
#include "RepLeanWin.h"
#include "RepMain.h"
#include "RepWinException.hpp"
#include "MoveOnlyBase.hpp"

namespace Replica
{
	class MinWindow;

	/// <summary>
	/// Base class for window components
	/// </summary>
	class WindowComponentBase : protected MoveOnlyObjBase
	{
	friend MinWindow;

	public:
		WindowComponentBase(WindowComponentBase&&) = default;
		WindowComponentBase& operator=(WindowComponentBase&&) = default;

		/// <summary>
		/// Returns pointer to parent window
		/// </summary>
		MinWindow& GetParent() const;

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

		WindowComponentBase();
		WindowComponentBase(MinWindow& window);

	private:
		MinWindow* pParent;
		bool isRegistered;
	};
}