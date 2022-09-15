#pragma once
#include <glm/glm.hpp>
#include "WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

using KbKey = DirectX::Keyboard::Keys;
using MouseMode = DirectX::Mouse::Mode;

enum class MouseKey : unsigned int
{
	None = 0x0,
	LeftButton = 0x1,
	MiddleButton = 0x2,
	RightButton = 0x4,
	xButton1 = 0x8,
	xButton2 = 0x10
};

BITWISE_ALL(MouseKey, unsigned int)

namespace Replica
{
	class InputHandler : public WindowComponentBase
	{
	using ivec2 = glm::ivec2;
	using vec2 = glm::vec2;

	public:
		InputHandler(MinWindow& window);

		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		/// <summary>
		/// Returns pressed mouse keys as a set of enum flags
		/// </summary>
		MouseKey GetPresssedMouseKeys() const;

		/// <summary>
		/// Returns previously pressed mouse keys
		/// </summary>
		MouseKey GetLastPressedMouseKeys() const;

		/// <summary>
		/// Returns true if the given key was only just pressed
		/// </summary>
		bool GetIsNewKeyPressed(MouseKey key) const;

		/// <summary>
		/// Returns true if the given key was previously pressed
		/// </summary>
		bool GetWasKeyPressed(MouseKey key) const;

		/// <summary>
		/// Returns true if a given mouse key is pressed
		/// </summary>
		bool GetIsKeyPressed(MouseKey key) const;

		/// <summary>
		/// Returns true if a given keyboard key is pressed
		/// </summary>
		bool GetIsKeyPressed(KbKey key) const;

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

		MouseKey currentMousePresses,
			lastMousePresses;
	};
}