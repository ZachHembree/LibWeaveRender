#pragma once
#include "WeaveUtils/Utils.hpp"

namespace Weave::D3D11
{
	enum class WindowRenderModes : byte
	{
		/// <summary>
		/// Disables automatic window mode management and resolution scaling
		/// </summary>
		None = 0,

		/// <summary>
		/// Automatically matches output resolution to window body size and sets bordered window style
		/// </summary>
		Windowed = 1,

		/// <summary>
		/// Disables window borders, sets body size and output resolution to match the desktop resolution, and 
		/// positions the window to fill the screen
		/// </summary>
		BorderlessFS = 2,

		/// <summary>
		/// Exclusive full screen mode
		/// </summary>
		ExclusiveFS = 3
	};

	/// <summary>
	/// An array of WindowRenderMode-Name pairs
	/// </summary>
	static constexpr std::array<std::pair<WindowRenderModes, string_view>, 4> g_WindowRenderModeNamePairs
	{
		std::pair(WindowRenderModes::None, "None"),
		std::pair(WindowRenderModes::Windowed, "Windowed"),
		std::pair(WindowRenderModes::BorderlessFS, "Borderless Fullscreen"),
		std::pair(WindowRenderModes::ExclusiveFS, "Exclusive Fullscreen")
	};

	/// <summary>
	/// Returns a null-terminated string_view representing the name of the given window mode enum
	/// </summary>
	inline static constexpr string_view GetWindowRenderModeName(WindowRenderModes mode)
	{
		return g_WindowRenderModeNamePairs[(uint)mode].second;
	}
}