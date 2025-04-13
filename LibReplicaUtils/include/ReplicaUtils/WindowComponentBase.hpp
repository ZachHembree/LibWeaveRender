#pragma once
#include "ReplicaUtils/RepWinException.hpp"
#include "ReplicaUtils/GlobalUtils.hpp"

namespace Replica
{
	class MinWindow;

	/// <summary>
	/// Base class for window components
	/// </summary>
	class WindowComponentBase
	{
	friend MinWindow;

	public:
		MAKE_MOVE_ONLY(WindowComponentBase)

		/// <summary>
		/// Returns pointer to parent window
		/// </summary>
		MinWindow& GetWindow() const;

		/// <summary>
		/// Called after each message poll in the window class
		/// </summary>
		virtual void Update() { }

		/// <summary>
		/// Invoked on WndProc callback. Interrupts update if the component returns false.
		/// </summary>
		virtual bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) { return true; }

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