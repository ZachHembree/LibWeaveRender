#ifndef REP_WIND_H
#define REP_WIND_H

#include "ReplicaWin32.hpp"

#pragma comment(lib, "Shcore.lib")

namespace Replica
{
	class WindowComponentBase;

	/// <summary>
	/// Minimal wrapper class for Win32 Window
	/// </summary>
	class MinWindow
	{
		public:
			MAKE_MOVE_ONLY(MinWindow)

			MinWindow(
				wstring_view name, 
				ivec2 initSize, 
				WndStyle initStyle, 
				const HINSTANCE hInst, 
				const wchar_t* iconRes
			);

			~MinWindow();

			/// <summary>
			/// Registers component object to the window.
			/// </summary>
			virtual void RegisterComponent(WindowComponentBase& component);

			/// <summary>
			/// Updates window message loop until the window is closed
			/// </summary>
			virtual MSG RunMessageLoop();

			/// <summary>
			/// Returns the name of the window
			/// </summary>
			const wstring_view GetName() const noexcept;

			/// <summary>
			/// Returns handle to executable process associated with the window
			/// </summary>
			HINSTANCE GetProcHandle() const noexcept;

			/// <summary>
			/// Returns handle for window object provided by the Win32 API
			/// </summary>
			HWND GetWndHandle() const noexcept;

			/// <summary>
			/// Returns true if the window is in borderless fullscreen mode
			/// </summary>
			bool GetIsFullScreen() const;

			/// <summary>
			/// Enable/disable borderless full screen
			/// </summary>
			void SetFullScreen(bool value);

			/// <summary>
			/// Returns the size of the window in pixels.
			/// </summary>
			ivec2 GetSize() const;

			/// <summary>
			/// Resizes the window to the given dimensions in pixels.
			/// </summary>
			void SetSize(ivec2 size);

			/// <summary>
			/// Returns the size of the window's body in pixels.
			/// </summary>
			ivec2 GetBodySize() const;
			
			/// <summary>
			/// Resizes the window body to the given dimensions in pixels.
			/// </summary>
			void SetBodySize(ivec2 size);

			/// <summary>
			/// Returns the position of the window's top right corner in pixels
			/// </summary>
			ivec2 GetPos() const;

			/// <summary>
			/// Sets the position of the window's top right corner in pixels
			/// </summary>
			void SetPos(ivec2 pos);

			/// <summary>
			/// Returns the contents of the titlebar
			/// </summary>
			wstring GetWindowTitle() const;

			/// <summary>
			/// Writes the given text to the titlebar
			/// </summary>
			void SetWindowTitle(wstring_view text);

			/// <summary>
			/// Returns style and extended style as a vector
			/// </summary>
			WndStyle GetStyle() const;

			/// <summary>
			/// Sets the style of the window. Ex-style optional.
			/// </summary>
			void SetStyle(WndStyle style);

			/// <summary>
			/// Adds the given style flags to the window
			/// </summary>
			void EnableStyleFlags(WndStyle flags);

			/// <summary>
			/// Removes the given style flags from the current style
			/// </summary>
			void DisableStyleFlags(WndStyle flags);

			/// <summary>
			/// Returns true if the application should allow the display to turn off
			/// while its running.
			/// </summary>
			bool GetCanDisplaySleep() const;

			/// <summary>
			/// Determines whether the application should allow the display to turn
			/// off while its running.
			/// </summary>
			void SetCanDisplaySleep(bool value);

			/// <summary>
			/// Returns true if the application should allow the system to sleep
			/// while its running.
			/// </summary>
			bool GetCanSystemSleep() const;

			/// <summary>
			/// Determines whether the application should allow the system to sleep
			/// while its running.
			/// </summary>
			void SetCanSystemSleep(bool value);

			/// <summary>
			/// Returns fractional, floating-point, DPI normalized to 96 DPI
			/// </summary>
			vec2 GetNormMonitorDPI() const;

			/// <summary>
			/// Returns the effective DPI of the monitor occupied by the window
			/// </summary>
			ivec2 GetMonitorDPI() const;

			/// <summary>
			/// Returns the virtual coordinates for the top left corner of the current monitor
			/// </summary>
			ivec2 GetMonitorPosition() const;

			/// <summary>
			/// Returns the current resolution of the monitor occupied by the window
			/// </summary>
			ivec2 GetMonitorResolution() const;

			/// <summary>
			/// Returns true if the mouse is inside the bounds of the window's client area
			/// </summary>
			bool GetIsMousedOver() const;

			/// <summary>
			/// Returns true if the cursor can be drawn inside the client region
			/// </summary>
			bool GetIsCursorVisible() const;

			/// <summary>
			/// Enables/disables the cursor inside the window's client region
			/// </summary>
			void SetIsCursorVisible(bool value);

		protected:	
			static MinWindow* pLastInit;

			const wstring_view name;
			HINSTANCE hInst;
			HWND hWnd;
			MSG wndMsg;
			ivec2 bodySize, wndSize;
			TRACKMOUSEEVENT tme;
			bool isInitialized;
			bool isMousedOver;
			bool canSysSleep;
			bool canDispSleep;

			bool isFullscreen;
			ivec2 lastPos, lastSize;
			WndStyle initStyle;

			/// <summary>
			/// Component objects associated with the window
			/// </summary>
			UniqueVector<WindowComponentBase*> components;

			/// <summary>
			/// Processes next window message without removing it from the queue.
			/// Returns false only on exit.
			/// </summary>
			virtual bool PollWindowMessages();

			/// <summary>
			/// Proceedure for processing window messages sent from Win32 API
			/// </summary>
			LRESULT OnWndMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

			void OnResize();

			/// <summary>
			/// Handles messaging setup on creation of new windows
			/// </summary>
			static LRESULT CALLBACK HandleWindowSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

			/// <summary>
			/// Forwards messages from the Win32 API to the appropriate window instance
			/// </summary>
			static LRESULT CALLBACK WindowMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}

#endif