#ifndef REP_WIND_H
#define REP_WIND_H

#include "RepLeanWin.h"
#include "RepMain.h"
#include "RepWinException.hpp"
#include "WindowComponentBase.hpp"

#include <glm/glm.hpp>
#include "DynamicCollections.hpp"

namespace Replica
{
	class WindowComponentBase;
	class MinWindow;

	/// <summary>
	/// Minimal wrapper class for windows
	/// </summary>
	class MinWindow
	{
		public:
			MinWindow(const HINSTANCE hInst, const glm::ivec2 size);

			MinWindow(MinWindow&& other) noexcept;

			MinWindow(const MinWindow&) = delete;

			MinWindow& operator=(MinWindow&& rhs) noexcept;

			MinWindow& operator=(const MinWindow&) = delete;

			~MinWindow();

			/// <summary>
			/// Registers component object to the window.
			/// </summary>
			virtual void RegisterComponent(WindowComponentBase* component);

			/// <summary>
			/// Updates window message loop until the window is closed
			/// </summary>
			virtual MSG RunMessageLoop();

			/// <summary>
			/// Returns the name of the window
			/// </summary>
			const wchar_t* GetName() const noexcept;

			/// <summary>
			/// Returns handle to executable process associated with the window
			/// </summary>
			HINSTANCE GetProcHandle() const noexcept;

			/// <summary>
			/// Returns handle for window object provided by the Win32 API
			/// </summary>
			HWND GetWndHandle() const noexcept;

		protected:		
			HINSTANCE hInst;
			HWND hWnd;
			MSG wndMsg;

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