#pragma once
#include "WeaveUtils/Win32.hpp"
#include "WeaveUtils/Math.hpp"
#include <d3d11_4.h>
#include "Resources/ResourceEnums.hpp"
#include "../D3DException.hpp"
#include "../D3DUtils.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Defines the target Direct3D feature level for the application (D3D_FEATURE_LEVEL_11_1).
	/// </summary>
	constexpr D3D_FEATURE_LEVEL g_FeatureLevel = D3D_FEATURE_LEVEL_11_1;

#ifndef NDEBUG
	/// <summary>
	/// Specifies device creation flags. In debug builds, enables the D3D11 debug layer.
	/// </summary>
	constexpr uint g_DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	/// <summary>
	/// Specifies device creation flags. In release builds, no special flags are used.
	/// </summary>
	constexpr uint g_DeviceFlags = 0x0u;
#endif // !NDEBUG

	/// <summary>
	/// Defines the required alignment for constant buffers in bytes (typically 16 bytes for D3D11).
	/// </summary>
	constexpr size_t g_ConstantBufferAlignment = 16;

	/// <summary>
	/// Defines the total number of programmable shading stages (Vertex, Hull, Domain, Geometry, Pixel) plus the Compute shader stage.
	/// </summary>
	static constexpr uint g_ShadeStageCount = 6;

	/// <summary>
	/// Retrieves the string representation of a DXGI_MODE_SCALING enumeration value.
	/// </summary>
	string_view GetScalingModeName(DXGI_MODE_SCALING mode);

	/// <summary>
	/// Retrieves the string representation of a DXGI_ALPHA_MODE enumeration value.
	/// </summary>
	string_view GetAlphaModeName(DXGI_ALPHA_MODE mode);

	/// <summary>
	/// Retrieves the string representation of a DXGI_SCALING enumeration value.
	/// </summary>
	string_view GetScalingModeName(DXGI_SCALING mode);

	/// <summary>
	/// Retrieves the string representation of a DXGI_SWAP_EFFECT enumeration value.
	/// </summary>
	string_view GetSwapModeName(DXGI_SWAP_EFFECT mode);

	/// <summary>
	/// Retrieves the string representation of a DXGI_SWAP_CHAIN_FLAG enumeration value.
	/// </summary>
	string_view GetSwapChainFlagName(DXGI_SWAP_CHAIN_FLAG flag);

	/// <summary>
	/// Retrieves the string representation of a D3D_FEATURE_LEVEL enum
	/// </summary>
	string_view GetFeatureLevelName(D3D_FEATURE_LEVEL level);
}