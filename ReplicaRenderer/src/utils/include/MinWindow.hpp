#ifndef REP_WIND_H
#define REP_WIND_H

#include "RepLeanWin.h"
#include "RepWinException.hpp"

#include <glm/glm.hpp>
#include "DynamicCollections.hpp"
#include <string_view>

#pragma comment(lib, "Shcore.lib")

namespace Replica
{
	typedef glm::tvec2<DWORD> WndStyle;
	typedef glm::tvec2<UINT> uivec2;
	
	using glm::ivec2;
	using glm::vec2;
	using std::string_view;
	using std::wstring_view;
	using std::string;
	using std::wstring;

	class WindowComponentBase;

	/// <summary>
	/// Minimal wrapper class for Win32 Window
	/// </summary>
	class MinWindow
	{
		public:
			MinWindow(
				wstring_view name, 
				ivec2 initSize, 
				WndStyle initStyle, 
				const HINSTANCE hInst, 
				const wchar_t* iconRes
			);

			MinWindow(MinWindow&& other) noexcept;

			MinWindow(const MinWindow&) = delete;

			MinWindow& operator=(MinWindow&& rhs) noexcept;

			MinWindow& operator=(const MinWindow&) = delete;

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

		protected:	
			static MinWindow* pLastInit;

			const wstring_view name;
			HINSTANCE hInst;
			HWND hWnd;
			MSG wndMsg;
			ivec2 bodySize, wndSize;

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