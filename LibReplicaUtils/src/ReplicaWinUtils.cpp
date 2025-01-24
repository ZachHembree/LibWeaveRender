#include "pch.hpp"
#include "ReplicaWinUtils.hpp"

namespace Replica
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

		WIN_ASSERT_NZ_LAST(size);
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

		WIN_ASSERT_NZ_LAST(size);
		return (size_t)size;
	}

	string GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src)
	{
		size_t dstSize = GetMultiByteSize_UTF16LE_TO_UTF8(src);
		string dst(dstSize + 1, '\0');

		WIN_ASSERT_NZ_LAST(WideCharToMultiByte(
			CP_UTF8,  // Code page
			0, // No additional flags
			&src[0], (int)src.size(), // Wide src
			&dst[0], (int)dst.size(), // Narrow dst
			NULL, NULL // Default/fallback chars, invalid for UTF8
		));

		return dst;
	}

	wstring GetWideString_UTF8_TO_UTF16LE(string_view src)
	{
		size_t dstSize = GetWideSize_UTF8_TO_UTF16LE(src);
		wstring dst(dstSize + 1, '\0');

		WIN_ASSERT_NZ_LAST(MultiByteToWideChar(
			CP_UTF8, // Code page
			0, // No additional flags
			&src[0], (int)src.size(), // Narrow src	
			&dst[0], (int)dst.size() // Wide dst
		));

		return dst;
	}
}