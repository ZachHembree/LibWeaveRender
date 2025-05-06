#pragma once
#include "WinUtils.hpp"
#include "ComponentManagerBase.hpp"

namespace Weave
{
	class MinWindow;

	/// <summary>
	/// Base class for MinWindow components
	/// </summary>
	class WindowComponentBase : public ComponentManagerBase<MinWindow, WindowComponentBase>::ComponentBase
	{
	public:
		MAKE_IMMOVABLE(WindowComponentBase);

		WindowComponentBase(MinWindow& parent, uint priority);

		/// <summary>
		/// Returns a reference to the parent window
		/// </summary>
		MinWindow& GetWindow() const;

		/// <summary>
		/// Called after each message poll in the window class
		/// </summary>
		virtual void Update();

		/// <summary>
		/// Invoked on WndProc callback. Interrupts update if the component returns false.
		/// </summary>
		virtual bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam);
	};
}