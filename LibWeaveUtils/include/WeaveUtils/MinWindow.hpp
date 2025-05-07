#pragma once
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/Math.hpp"
#include "ComponentManagerBase.hpp"
#include "WinUtils.hpp"

namespace Weave
{
	class WindowComponentBase;

	typedef glm::tvec2<uint> WndStyle;
	constexpr WndStyle g_NullStyle(-1, -1);

	/// <summary>
	/// Describes the configuration of a given monitor as reported by Win32
	/// </summary>
	struct WndMonConfig
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
	/// Enumerates hit test overrides for custom non client areas
	/// </summary>
	enum class WndHitTestOverrides : byte
	{
		None = 0,

		/// <summary>
		/// Used to indicate whether the window header is moused over
		/// </summary>
		Caption = 1,

		/// <summary>
		/// Used to indicate whether the minimize button is moused over
		/// </summary>
		Minimize = 2,

		/// <summary>
		/// Used to indicate whether the maximize button is moused over
		/// </summary>
		Maximize = 3,

		/// <summary>
		/// Used to indicate whether the close button is moused over
		/// </summary>
		Close = 4
	};

	/// <summary>
	/// Defines a style override for a custom non-client area for MinWindow to be painted by the client.
	/// </summary>
	class WndStyleOverride
	{
	public:
		WndStyleOverride(uivec2 padding = uivec2(10)) :
			padding(padding),
			htOverride(WndHitTestOverrides::None)
		{ }

		/// <summary>
		/// Returns the logical thickness of the border around the window to be used for resizing
		/// </summary>
		uivec2 GetPadding() const;

		/// <summary>
		/// Sets the logical thickness of the border around the window to be used for resizing
		/// </summary>
		void SetPadding(uivec2 padding);

		/// <summary>
		/// Returns an enum indicating an override for a non-client area hit test override. These are used to 
		/// indicate when the header, close, minimize, maximize buttons of a custom non-client area are moused
		/// over. Override enums are mutually exclusive and must be cleared manually.
		/// </summary>
		WndHitTestOverrides GetHitTestOverride() const;

		/// <summary>
		/// Sets an enum indicating an override for a non-client area hit test override. These are used to 
		/// indicate when the header, close, minimize, maximize buttons of a custom non-client area are moused
		/// over. Override enums are mutually exclusive and must be cleared manually.
		/// </summary>
		void SetHitTestOverride(WndHitTestOverrides hitTest, bool isSet = true);

	private:
		std::atomic<uivec2> padding;
		std::atomic<WndHitTestOverrides> htOverride;
	};

	/// <summary>
	/// Unique pointer containing a MinWindow component
	/// </summary>
	using WndCompHandle = std::unique_ptr<WindowComponentBase>;

	/// <summary>
	/// Minimal wrapper class for Win32 Window
	/// </summary>
	class MinWindow : public ComponentManagerBase<MinWindow, WindowComponentBase>
	{
		public:
			MAKE_MOVE_ONLY(MinWindow);

			MinWindow();

			/// <summary>
			/// Creates a window with the given styling
			/// </summary>
			MinWindow(
				wstring_view name,
				ivec2 bodySize,
				const HINSTANCE hInst,
				WndStyle style = g_NullStyle,
				const wchar_t* iconRes = nullptr
			);

			/// <summary>
			/// Creates a window with an overridden non-client area
			/// </summary>
			MinWindow(
				wstring_view name,
				ivec2 bodySize,
				const HINSTANCE hInst,
				const WndStyleOverride& styleOverride,
				const wchar_t* iconRes = nullptr
			);

			~MinWindow();

			/// <summary>
			/// Updates main Window loop until the application exits. To be run once from the owning thread.
			/// </summary>
			void RunMessageLoop(MSG& msg);

			/// <summary>
			/// Returns the contents of the titlebar
			/// </summary>
			void GetWindowTitle(wstring& title) const;

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
			/// Returns true if the non-client area of the window has been overridden
			/// </summary>
			bool GetIsStyleOverridden() const;

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
			uivec2 GetSize() const;

			/// <summary>
			/// Resizes the window to the given dimensions in pixels.
			/// </summary>
			void SetSize(uivec2 size);

			/// <summary>
			/// Returns the size of the window's body in pixels.
			/// </summary>
			uivec2 GetBodySize() const;

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
			WndMonConfig GetActiveMonitorConfig() const;

			/// <summary>
			/// Returns configuration details for the given monitor including resolution, position,
			/// refresh rate and bits per pixel.
			/// </summary>
			static WndMonConfig GetMonitorConfig(HMONITOR mon);

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

		protected:	
			friend WindowComponentBase;
			mutable std::shared_mutex wndMutex;

			HWND hWnd;
			wstring name;
			HINSTANCE hInst;
			std::atomic<bool> isInitialized;
			bool isMoving;
			ulong timerID;

			// Styling
			WndStyle style;
			std::atomic<uivec2> bodySize, wndSize;
			std::atomic<ivec2> wndPos, bodyPos;

			std::atomic<bool> isFullscreen;
			uivec2 lastSize;
			ivec2 lastPos;

			// Mouse tracking
			std::unique_ptr<TRACKMOUSEEVENT> pMouseTrackEvent;
			std::atomic<bool> isMousedOver;
			std::atomic<bool> canSysSleep;
			std::atomic<bool> canDispSleep;

			// Non-client override
			const WndStyleOverride* pStyleOverride;

			MinWindow(
				wstring_view name,
				ivec2 initSize,
				const HINSTANCE hInst,
				const wchar_t* iconRes,
				WndStyle style,
				const WndStyleOverride* pStyleOverride
			);

			/// <summary>
			/// Sets the style of the window. Ex-style optional.
			/// </summary>
			void SetStyle(WndStyle style);

			/// <summary>
			/// Manually triggers an update and repaint of the window
			/// </summary>
			void RepaintWindow();

			/// <summary>
			/// Runs component update tick
			/// </summary>
			void UpdateComponents();

			/// <summary>
			/// Processes next window message without removing it from the queue.
			/// Returns false only on exit.
			/// </summary>
			bool PollWindowMessages(MSG& msg);

			/// <summary>
			/// Proceedure for processing window messages sent from Win32 API
			/// </summary>
			slong OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam);

			/// <summary>
			/// Checks Win32 messages for changes to the window
			/// </summary>
			void TrackState(HWND hWnd, uint msg, ulong wParam, slong lParam);

			/// <summary>
			/// Overrides non-client area styling Win32 messages if enabled
			/// </summary>
			std::optional<slong> TryHandleOverrideNC(HWND hWnd, uint msg, ulong wParam, slong lParam);

			/// <summary>
			/// Handles changes to sizing and position
			/// </summary>
			std::optional<slong> TryHandleMove(HWND hWnd, uint msg, ulong wParam, slong lParam);

			/// <summary>
			/// Updates window and body size
			/// </summary>
			void UpdateSize();

			/// <summary>
			/// Handles messaging setup on creation of new windows
			/// </summary>
			static slong CALLBACK HandleWindowSetup(HWND hWnd, uint msg, ulong wParam, slong lParam);

			/// <summary>
			/// Forwards messages from the Win32 API to the appropriate window instance
			/// </summary>
			static slong CALLBACK WindowMessageHandler(HWND hWnd, uint msg, ulong wParam, slong lParam);
	};
}
