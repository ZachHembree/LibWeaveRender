#pragma once
#include <glm/glm.hpp>
#include "WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

namespace Replica
{
	class InputComponent : public WindowComponentBase
	{
	public:
		InputComponent(MinWindow* window) : WindowComponentBase(window) { }

		void OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		glm::ivec2 GetMousePos();
		
		glm::vec2 GetNormMousePos();

	private:
		DirectX::Keyboard keyboard;
		DirectX::Mouse mouse;

	};
}