#pragma once
#include "../src/utils/include/RepLeanWin.h"
#include <wrl.h>
#include <glm/glm.hpp>

namespace Replica
{
	typedef glm::tvec2<DWORD> WndStyle;
	typedef glm::tvec2<UINT> uivec2;

	using glm::ivec2;
	using glm::vec2;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}

#include "ReplicaUtils.hpp"