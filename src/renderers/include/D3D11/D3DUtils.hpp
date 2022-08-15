#pragma once
#include "GfxException.hpp"
#include <glm/glm.hpp>
#include <wrl.h>
#include "UniqueObj.hpp"

namespace Replica::D3D11
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	using vec2 = glm::vec2;
	using vec4 = glm::vec4;

}