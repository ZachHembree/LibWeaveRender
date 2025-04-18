#pragma once
// Just trimming some fat from the windows headers
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOMINMAX

#include <wrl.h>
#include <cstdint>
#include <glm/glm.hpp>
#include "WeaveUtils/Utils.hpp"

namespace Weave
{
	typedef glm::tvec2<DWORD> WndStyle;
	typedef glm::tvec2<uint> uivec2;

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