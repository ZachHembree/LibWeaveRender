#include "pch.hpp"
#include "WeaveUtils/WinUtils.hpp"
#include "WeaveUtils/WeaveWinException.hpp"

namespace Weave
{
	size_t GetMultiByteSize_UTF16LE_TO_UTF8(wstring_view src)
	{
		int size = WideCharToMultiByte(
			CP_UTF8,  // Code page
			0, // No additional flags
			src.data(), (int)src.size(), // Wide src
			nullptr, 0, // Narrow dst
			NULL, NULL // Default/fallback chars, invalid for UTF8
		);

		WIN_CHECK_NZ_LAST(size);
		return (size_t)size;
	}

	size_t GetWideSize_UTF8_TO_UTF16LE(string_view src)
	{
		int size = MultiByteToWideChar(
			CP_UTF8, // Code page
			0, // No additional flags
			src.data(), (int)src.size(), // Narrow src	
			nullptr, 0 // Wide dst
		);

		WIN_CHECK_NZ_LAST(size);
		return (size_t)size;
	}

	void GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src, string& dst)
	{
		size_t dstSize = GetMultiByteSize_UTF16LE_TO_UTF8(src);
		dst.clear();
		dst.resize(dstSize, '\0');

		WIN_CHECK_NZ_LAST(WideCharToMultiByte(
			CP_UTF8,  // Code page
			0, // No additional flags
			&src[0], (int)src.size(), // Wide src
			&dst[0], (int)dst.size(), // Narrow dst
			NULL, NULL // Default/fallback chars, invalid for UTF8
		));
	}

	string GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src)
	{
		string dst;
		GetMultiByteString_UTF16LE_TO_UTF8(src, dst);
		return dst;
	}

	void GetWideString_UTF8_TO_UTF16LE(string_view src, wstring& dst)
	{
		size_t dstSize = GetWideSize_UTF8_TO_UTF16LE(src);
		dst.clear();
		dst.resize(dstSize, '\0');

		WIN_CHECK_NZ_LAST(MultiByteToWideChar(
			CP_UTF8, // Code page
			0, // No additional flags
			&src[0], (int)src.size(), // Narrow src	
			&dst[0], (int)dst.size() // Wide dst
		));
	}

	wstring GetWideString_UTF8_TO_UTF16LE(string_view src)
	{
		wstring dst;
		GetWideString_UTF8_TO_UTF16LE(src, dst);
		return dst;
	}
}