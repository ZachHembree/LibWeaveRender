#pragma once
#include "WeaveUtils/WeaveWinException.hpp"
#include "WeaveUtils/GlobalUtils.hpp"

namespace Weave
{
	class MinWindow;

	/// <summary>
	/// Base class for MinWindow components
	/// </summary>
	class WindowComponentBase
	{
	friend MinWindow;

	public:
		MAKE_IMMOVABLE(WindowComponentBase);

		WindowComponentBase(MinWindow& parent, uint priority = 10);

		virtual ~WindowComponentBase() = 0;

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

	private:
		MinWindow* pParent;
		uint id;
		uint priority;
	};

	inline WindowComponentBase::~WindowComponentBase() {}
}