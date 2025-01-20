#include "internal/Utils.hpp"
#include <algorithm>

namespace Replica
{
	/// <summary>
	/// Returns unsigned difference (a - b) between two unsigned integers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(size_t a, size_t b)
	{
		return (a >= b) * (a - b);
	}

	/// <summary>
	/// Returns unsigned difference (a - b) between two pointers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(const void* a, const void* b)
	{
		return UnsignedDelta((size_t)a, (size_t)b);
	}

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(size_t a, size_t b)
	{
		const long sign = 2 * (a >= b) - 1;
		return sign * (long)(std::max(a, b) - std::min(a, b));
	}

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(const void* a, const void* b)
	{
		return Delta((size_t)a, (size_t)b);
	}
}