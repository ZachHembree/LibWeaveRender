#pragma once
#include "ReplicaD3D11.hpp"

#include "../src/renderers/include/D3D11/ImguiRenderComponent.hpp"

#include "../src/renderers/include/D3D11/Resources/BufferBase.hpp"
#include "../src/renderers/include/D3D11/Resources/ConstantBuffer.hpp"

#include "../src/renderers/include/D3D11/Shaders/BuiltInShaders.hpp"
#include "../src/renderers/include/D3D11/Shaders/VertexShader.hpp"
#include "../src/renderers/include/D3D11/Shaders/PixelShader.hpp"

#include "../src/renderers/include/D3D11/Device.hpp"
#include "../src/renderers/include/D3D11/Context.hpp"
#include "../src/renderers/include/D3D11/SwapChain.hpp"
#include "../src/renderers/include/D3D11/Resources/RTHandle.hpp"

namespace Replica::D3D11
{
	constexpr size_t g_ConstantBufferAlignment = 16;
}