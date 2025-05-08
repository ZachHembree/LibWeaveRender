#pragma once
#include <functional>
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

	/// <summary>
	/// Generic window component wrapper that provides std::function hooks for update ticks and Win32 messages
	/// </summary>
	class WindowHook : public WindowComponentBase
	{
	public:
		using UpdateCallbackT = std::function<void()>;
		using MsgCallbackT = std::function<bool(HWND hWnd, uint msg, ulong wParam, slong lParam)>;

		WindowHook(MinWindow& wnd, uint priority = 10);

		/// <summary>
		/// Returns the callback used for component update ticks
		/// </summary>
		const UpdateCallbackT& GetUpdateCallback() const;

		/// <summary>
		/// Sets the callback to be invoked on component update ticks
		/// </summary>
		void SetUpdateCallback(UpdateCallbackT&& func);

		/// <summary>
		/// Returns the callback used for intercepting Win32 messages
		/// </summary>
		const MsgCallbackT& GetMsgCallback() const;

		/// <summary>
		/// Sets the callback to be invoked when the window is processing Win32 messages
		/// </summary>
		void SetMsgCallback(MsgCallbackT&& func);

	private:
		UpdateCallbackT updateFunc;
		MsgCallbackT msgFunc;

		void Update() override;

		bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) override;
	};
}