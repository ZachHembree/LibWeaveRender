#pragma once

namespace Weave
{
	/// <summary>
	/// Returns unsigned difference (a - b) between two unsigned integers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(size_t a, size_t b);

	/// <summary>
	/// Returns unsigned difference (a - b) between two pointers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(const void* a, const void* b);

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(size_t a, size_t b);

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(const void* a, const void* b);
}