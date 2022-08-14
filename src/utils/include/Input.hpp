#pragma once
#include <glm/glm.hpp>
#include "WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

using KbKey = DirectX::Keyboard::Keys;

namespace Replica
{
	class InputComponent : public WindowComponentBase
	{
	using ivec2 = glm::ivec2;
	using vec2 = glm::vec2;

	public:
		InputComponent(MinWindow* window) :
			WindowComponentBase(window) 
		{ }

		void OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		/// <summary>
		/// Returns true if a given keyboard key is pressed
		/// </summary>
		bool GetIsKeyPressed(KbKey key);;

		/// <summary>
		/// Returns raw, pixel-value, mouse position relative to the upper left corner
		/// of the window
		/// </summary>
		ivec2 GetMousePos() const;
		
		/// <summary>
		/// Returns normalized, [0, 1] mouse position relative to the upper left corner
		/// of the window
		/// </summary>
		vec2 GetNormMousePos() const;

	private:
		DirectX::Keyboard keyboard;
		DirectX::Mouse mouse;

	};
}