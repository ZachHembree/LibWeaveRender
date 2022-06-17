#ifndef REP_WIND_H
#define REP_WIND_H

#include "RepLeanWin.h"
#include "RepMain.h"
#include "RepWinException.hpp"
#include <vector>
#include <glm/glm.hpp>
#include "UniqueCollections.hpp"

typedef void (*MinWndUpdate)();

class WindowComponentBase
{
public: 
	virtual void Update() = 0;

	WindowComponentBase(const WindowComponentBase&) = delete;

	WindowComponentBase(WindowComponentBase&&) = delete;

	WindowComponentBase& operator=(const WindowComponentBase&) = delete;

	WindowComponentBase& operator=(WindowComponentBase&&) = delete;

private:
	WindowComponentBase();

};

class MinWindow
{
	public:
		MinWindow(const HINSTANCE hInst, const glm::ivec2 size);

		MinWindow(MinWindow&& other) noexcept;

		MinWindow(const MinWindow&) = delete;

		MinWindow& operator=(MinWindow&& rhs) noexcept;

		MinWindow& operator=(const MinWindow&) = delete;

		~MinWindow();

		MSG RunMessageLoop();

		const wchar_t* GetName() const noexcept;

		HINSTANCE GetProcHandle() const noexcept;

		HWND GetWndHandle() const noexcept;

		MSG GetLastWndMessage() const;

	private:		
		HINSTANCE hInst;
		HWND hWnd;
		MSG wndMsg;

		UniqueVector<WindowComponentBase*> components;

		bool PollWindowMessages();

		static LRESULT CALLBACK OnWndMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif