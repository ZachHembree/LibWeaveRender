#pragma once
#include "WeaveUtils/WinUtils.hpp"
#include "WeaveUtils/Math.hpp"
#include <d3d11.h>

#include "../src/renderers/include/D3DException.hpp"

namespace Weave::D3D11
{
	constexpr D3D_FEATURE_LEVEL g_FeatureLevel = D3D_FEATURE_LEVEL_11_1;

#ifndef NDEBUG
	constexpr uint g_DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
	constexpr uint g_DeviceFlags = 0x0u;
#endif // !NDEBUG

	constexpr size_t g_ConstantBufferAlignment = 16;
}