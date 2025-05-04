#pragma once
#include "WeaveUtils/WeaveWinException.hpp"
#include "WeaveUtils/GlobalUtils.hpp"

namespace Weave
{
	class MinWindow;

	/// <summary>
	/// Base class for window components. Derived types must explicitly invoke base constructors and move operations
	/// to register and update registration for the component.
	/// </summary>
	class WindowComponentBase
	{
	friend MinWindow;

	public:
		MAKE_NO_COPY(WindowComponentBase)

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
		virtual bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		/// <summary>
		/// Returns true if the component has been registered to a window. If a parent window is specified, 
		/// it will return false if it is unregistered or registered to a window other than the given parent.
		/// </summary>
		bool GetIsRegistered(MinWindow* pParent = nullptr);

		/// <summary>
		/// Returns the update priority set for the component on construction. Components with higher priority values are 
		/// updated later. Components with a priority of 0 update soonest.
		/// </summary>
		uint GetPriority() const;

	protected:
		WindowComponentBase();

		/// <summary>
		/// Registers the component to the given window with the given update priority. Lower (sooner) priority values are 
		/// not recommended unless you specifically need to define an update interrupt or intercept messages from later 
		// components.
		/// </summary>
		WindowComponentBase(MinWindow& window, uint priority = 10);

		WindowComponentBase(WindowComponentBase&&) noexcept;

		WindowComponentBase& operator=(WindowComponentBase&&) noexcept;

	private:
		MinWindow* pParent;
		uint id;
		uint priority;
	};
}