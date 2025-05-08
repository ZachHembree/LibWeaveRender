#pragma once
#include <atomic>
#include <array>
#include "WeaveUtils/WindowComponentBase.hpp"
#include "Keyboard.h"
#include "Mouse.h"

namespace Weave
{
	using KbKey = DirectX::Keyboard::Keys;
	using MouseMode = DirectX::Mouse::Mode;

	enum class MouseKey : byte
	{
		None = 0x0,
		LeftButton = 0x1,
		MiddleButton = 0x2,
		RightButton = 0x4,
		xButton1 = 0x8,
		xButton2 = 0x10
	};

	/// <summary>
	/// Singleton wrapper for DirectXTK Keyboard and Mouse. Input reads thread safe.
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

		/// <summary>
		/// Returns true if input is enabled
		/// </summary>
		static bool GetIsEnabled();

		/// <summary>
		/// Enables or disables input
		/// </summary>
		static void SetIsEnabled(bool value);

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

		/// <summary>
		/// Retrieves complete input state
		/// </summary>
		static void GetInputState(KbState& kbState, MouseState& msState);

	private:
		class MouseTracker
		{
		public:
			MouseTracker() :
				currentMousePresses(MouseKey::None),
				lastMousePresses(MouseKey::None),
				lastMousePos(0),
				mousePos(0)
			{}

			ivec2 GetMousePos() const;

			ivec2 GetLastMousePos() const;

			ivec2 GetMousePosDelta() const;

			bool GetWasKeyPressed(MouseKey key) const;

			bool GetIsKeyPressed(MouseKey key) const;

			bool GetIsNewKeyPressed(MouseKey key) const;

			void Update(const MouseState& state);

			void Reset();

		private:
			MouseKey currentMousePresses,
				lastMousePresses;
			ivec2 lastMousePos;
			ivec2 mousePos;
		};

		friend ManagerT;
		static InputHandler* s_pHandler;
		static std::atomic<bool> s_IsEnabled;

		static constexpr uint g_TrackHistSize = 3;
		std::array<KbTracker, g_TrackHistSize> kbTrackers;
		std::array<KbState, g_TrackHistSize> kbStates;
		std::array<MouseTracker, g_TrackHistSize> msTrackers;
		std::array<MouseState, g_TrackHistSize> msStates;
		std::atomic<uint> writeIndex, readIndex;

		std::unique_ptr<Keyboard> keyboard;
		std::unique_ptr <Mouse> mouse;
		ivec2 lastMousePos;

		InputHandler(MinWindow& parent);

		~InputHandler();

		const KbTracker& ReadKeyboard() const;

		const MouseTracker& ReadMouse() const;

		void Update() override;

		bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) override;

	};
}