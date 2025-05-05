#pragma once
#include "WeaveUtils/WinUtils.hpp"

namespace Weave
{
	class WindowComponentBase;

	/// <summary>
	/// Describes the configuration of a given monitor as reported by Win32
	/// </summary>
	struct WinMonConfig
	{	
		/// <summary>
		/// Current resolution of the monitor. 
		/// X == width - Y == height
		/// </summary>
		uivec2 res;

		/// <summary>
		/// Position of the monitor's upper left corner
		/// </summary>
		ivec2 pos;

		/// <summary>
		/// Rounded or truncated monitor refresh rate in Hz
		/// </summary>
		uint refreshHz;

		/// <summary>
		/// Bits used for each pixel
		/// </summary>
		uint bitsPerPixel;
	};

	/// <summary>
	/// Unique pointer containing a MinWindow component
	/// </summary>
	using WndCompHandle = std::unique_ptr<WindowComponentBase>;

	/// <summary>
	/// Minimal wrapper class for Win32 Window
	/// </summary>
	class MinWindow
	{
		public:
			MAKE_MOVE_ONLY(MinWindow);

			MinWindow();

			MinWindow(
				wstring_view name, 
				ivec2 initSize, 
				WndStyle initStyle, 
				const HINSTANCE hInst, 
				const wchar_t* iconRes
			);

			~MinWindow();

			/// <summary>
			/// Updates window message loop until the window is closed
			/// </summary>
			MSG RunMessageLoop();

			/// <summary>
			/// Returns the contents of the titlebar
			/// </summary>
			wstring_view GetWindowTitle() const;

			/// <summary>
			/// Writes the given text to the titlebar
			/// </summary>
			void SetWindowTitle(wstring_view text);

			/// <summary>
			/// Returns the name of the window
			/// </summary>
			const wstring_view GetName() const;

			/// <summary>
			/// Returns handle to executable process associated with the window
			/// </summary>
			HINSTANCE GetProcHandle() const;

			/// <summary>
			/// Returns handle for window object provided by the Win32 API
			/// </summary>
			HWND GetWndHandle() const;

			/// <summary>
			/// Returns style and extended style as a vector
			/// </summary>
			WndStyle GetStyle() const;

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
			/// Returns the position of the upper left corner of the body in the window
			/// </summary>
			ivec2 GetBodyPos() const;

			/// <summary>
			/// Returns true if the mouse is inside the bounds of the window's client area
			/// </summary>
			bool GetIsMousedOver() const;

			/// <summary>
			/// Returns global cursor position
			/// </summary>
			ivec2 GetGlobalCursorPos() const;

			/// <summary>
			/// Returns true if the cursor can be drawn inside the client region
			/// </summary>
			bool GetIsCursorVisible() const;

			/// <summary>
			/// Enables/disables the cursor inside the window's client region
			/// </summary>
			void SetIsCursorVisible(bool value);

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
			/// Returns handle to the monitor the window is occupying
			/// </summary>
			HMONITOR GetActiveMonitor() const;

			/// <summary>
			/// Moves the window to the given monitor
			/// </summary>
			void SetActiveMonitor(HMONITOR newMon);

			/// <summary>
			/// Returns configuration details for the active monitor including resolution, position,
			/// refresh rate and bits per pixel.
			/// </summary>
			WinMonConfig GetActiveMonitorConfig() const;

			/// <summary>
			/// Returns configuration details for the given monitor including resolution, position,
			/// refresh rate and bits per pixel.
			/// </summary>
			static WinMonConfig GetMonitorConfig(HMONITOR mon);

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
			/// Returns true if the non client area is overridden
			/// </summary>
			bool GetIsNonClientCustom() const;

			/// <summary>
			/// Enables or disables the non client area override
			/// </summary>
			void SetIsNonClientCustom(bool value);

			/// <summary>
			/// Returns the custom border padding for non-client area overrides.
			/// </summary>
			ivec2 GetOverridePadding() const;

			/// <summary>
			/// Sets the custom border padding for non-client area overrides.
			/// </summary>
			void SetOverridePadding(ivec2 padding);

			/// <summary>
			/// Set to true to signal when a custom header is moused over
			/// </summary>
			void HoverHeader(bool isHovered);

			/// <summary>
			/// Minimizes the window as if the user had pressed the button
			/// </summary>
			void Minimize();

			/// <summary>
			/// Maximizes the window as if the user had pressed the button
			/// </summary>
			void Maximize();

			/// <summary>
			/// Closes the window and exits the application
			/// </summary>
			void CloseWindow();

			/// <summary>
			/// Constructs and registers a new window component in place
			/// </summary>
			template <typename T, typename... ArgTs>
			T& RegisterNewComponent(ArgTs... args)
			{
				WindowComponentBase* pComp = RegisterComponent(WndCompHandle(new T(args...)));
				WV_ASSERT(pComp != nullptr);
				return *static_cast<T*>(pComp);
			}

			/// <summary>
			/// Transfers ownership of the given component to the window and registers it
			/// </summary>
			template <typename T>
			T& RegisterNewComponent(T*&& pRawComp)
			{
				WindowComponentBase* pComp = RegisterComponent(WndCompHandle(pRawComp));
				WV_ASSERT(pComp != nullptr);
				pRawComp = nullptr;
				return *static_cast<T*>(pComp);
			}			

			/// <summary>
			/// Unregisters the component from the window and destroys it
			/// </summary>
			void UnregisterComponent(WindowComponentBase& component);

		protected:	
			friend WindowComponentBase;

			wstring name;
			mutable wstring title;
			HINSTANCE hInst;
			bool isInitialized;

			HWND hWnd;
			MSG wndMsg;

			// Styling
			WndStyle style;
			ivec2 bodySize, wndSize;
			bool isFullscreen;
			ivec2 lastPos, lastSize;

			// Components
			UniqueVector<WndCompHandle> components;
			bool isCompSortingStale;
			bool areCompIDsStale;

			// Mouse tracking
			TRACKMOUSEEVENT tme;
			bool isMousedOver;
			bool canSysSleep;
			bool canDispSleep;

			// Non-client override
			ivec2 paddingOverride;
			bool isHeaderHovered;
			bool isNcCustom;

			/// <summary>
			/// Registers component object to the window.
			/// </summary>
			WindowComponentBase* RegisterComponent(WndCompHandle&& component);

			/// <summary>
			/// Sets the style of the window. Ex-style optional.
			/// </summary>
			void SetStyle(WndStyle style);

			/// <summary>
			/// Manually triggers an update and repaint of the window
			/// </summary>
			void RepaintWindow();

			/// <summary>
			/// Processes next window message without removing it from the queue.
			/// Returns false only on exit.
			/// </summary>
			virtual bool PollWindowMessages();

			/// <summary>
			/// Proceedure for processing window messages sent from Win32 API
			/// </summary>
			LRESULT OnWndMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

			/// <summary>
			/// Overrides non-client area styling Win32 messages if enabled
			/// </summary>
			std::optional<slong> TryHandleOverrideNC(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

			/// <summary>
			/// Updates window and body size
			/// </summary>
			void UpdateSize();

			/// <summary>
			/// Updates component IDs and update ordering
			/// </summary>
			void UpdateComponentIDs();

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
