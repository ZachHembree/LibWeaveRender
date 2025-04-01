#pragma once
#include "ReplicaWinUtils.hpp"
#include "ReplicaMath.hpp"
#include <d3d11.h>

#include "../src/utils/include/GfxException.hpp"

namespace Replica::D3D11
{
	constexpr D3D_FEATURE_LEVEL g_FeatureLevel = D3D_FEATURE_LEVEL_11_1;

	constexpr D3D11_CREATE_DEVICE_FLAG g_DeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

	constexpr size_t g_ConstantBufferAlignment = 16;
}