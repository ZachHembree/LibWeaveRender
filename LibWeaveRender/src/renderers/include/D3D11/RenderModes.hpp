#pragma once
#include "WeaveUtils/GlobalUtils.hpp"

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
	/// A constexpr array of WindowRenderMode-Name pairs with null-termination
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

	enum class VSyncRenderModes : byte
	{
		/// <summary>
		/// No vertical sync. Requires ExclusiveFS mode or tearing support with BorderlessFS
		/// </summary>
		Disabled = 0,

		/// <summary>
		/// Triple buffered vsync
		/// </summary>
		TripleBuffered = 1,

		/// <summary>
		/// Variable rate refresh. Requires tearing support and full screen optimizations.
		/// </summary>
		VariableRefresh = 2
	};

	/// <summary>
	/// A constexpr array of VSyncRenderMode-Name pairs with null termination
	/// </summary>
	static constexpr std::array<std::pair<VSyncRenderModes, string_view>, 3> g_VSyncRenderModeNamePairs
	{
		std::pair(VSyncRenderModes::Disabled, "Disabled"),
		std::pair(VSyncRenderModes::TripleBuffered, "Triple Buffered"),
		std::pair(VSyncRenderModes::VariableRefresh, "Variable Rate Refresh")
	};

	/// <summary>
	/// Returns a null-terminated string_view representing the name of the given VSync enum
	/// </summary>
	inline static constexpr string_view GetVSyncRenderModeName(VSyncRenderModes mode)
	{
		return g_VSyncRenderModeNamePairs[(uint)mode].second;
	}
}