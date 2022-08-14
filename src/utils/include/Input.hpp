#pragma once
#include "WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

namespace Replica
{
	class InputComponent : public WindowComponentBase
	{
	public:
		void OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override 
		{
			
		}

	private:
		

	};
}