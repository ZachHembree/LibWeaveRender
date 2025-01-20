#pragma once
#include "RepLeanWin.h"
#include "RepWinException.hpp"

#include <wrl.h>
#include <cstdint>
#include <glm/glm.hpp>
#include "ReplicaUtils.hpp"

namespace Replica
{
	typedef glm::tvec2<DWORD> WndStyle;
	typedef glm::tvec2<UINT> uivec2;

	using glm::ivec2;
	using glm::vec2;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// Returns a copy of a given wide UTF-16LE string as a narrow UTF-8 string.
	/// </summary>
	string GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src);

	/// <summary>
	/// Returns a copy of a given narrow UTF-8 string as a UTF-16LE string.
	/// </summary>
	wstring GetWideString_UTF8_TO_UTF16LE(string_view src);
}