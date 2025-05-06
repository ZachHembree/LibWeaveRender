#pragma once
#include "WeaveUtils/WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

namespace Weave
{
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

	/// <summary>
	/// Singleton wrapper for DirectXTK Keyboard and Mouse
	/// </summary>
	class InputHandler : public WindowComponentBase
	{
	public:
		using Mouse = DirectX::Mouse;
		using Keyboard = DirectX::Keyboard;
		using KbTracker = Keyboard::KeyboardStateTracker;
		using MouseState = Mouse::State;
		using KbState = Keyboard::State;

		static void Init(MinWindow& wnd);

		static bool GetIsInitialized();

		void Update() override;

		bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) override;

		/// <summary>
		/// Returns true if input is enabled
		/// </summary>
		static bool GetIsEnabled();

		/// <summary>
		/// Enables or disables input
		/// </summary>
		static void SetIsEnabled(bool value);

		/// <summary>
		/// Returns pressed mouse keys as a set of enum flags
		/// </summary>
		static MouseKey GetPresssedMouseKeys();

		/// <summary>
		/// Returns previously pressed mouse keys
		/// </summary>
		static MouseKey GetLastPressedMouseKeys();

		/// <summary>
		/// Returns true if the given key was only just pressed
		/// </summary>
		static bool GetIsNewKeyPressed(MouseKey key);

		/// <summary>
		/// Returns true if the given key was previously pressed
		/// </summary>
		static bool GetWasKeyPressed(MouseKey key);

		/// <summary>
		/// Returns true if a given mouse key is pressed
		/// </summary>
		static bool GetIsKeyPressed(MouseKey key);

		/// <summary>
		/// Returns true if the given key was only just pressed
		/// </summary>
		static bool GetIsNewKeyPressed(KbKey key);

		/// <summary>
		/// Returns true if the given key was previously pressed
		/// </summary>
		static bool GetWasKeyPressed(KbKey key);

		/// <summary>
		/// Returns true if a given keyboard key is pressed
		/// </summary>
		static bool GetIsKeyPressed(KbKey key);

		/// <summary>
		/// Returns raw, pixel-value, mouse position relative to the upper left corner
		/// of the window
		/// </summary>
		static ivec2 GetMousePos();
		
		/// <summary>
		/// Returns normalized, [0, 1] mouse position relative to the upper left corner
		/// of the window
		/// </summary>
		static vec2 GetNormMousePos();

	private:
		friend ManagerT;
		static InputHandler* s_pHandler;
		static bool s_IsEnabled;

		KbTracker kbTracker;
		MouseKey currentMousePresses,
			lastMousePresses;

		std::unique_ptr<Keyboard> keyboard;
		std::unique_ptr <Mouse> mouse;
		ivec2 lastMousePos;
		bool isInitialized;

		InputHandler(MinWindow& parent);

		~InputHandler();
	};
}