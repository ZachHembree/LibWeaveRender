#include "D3D11/D3DUtils.hpp"

namespace Replica::D3D11
{ 
	fquat QuatFromAxis(vec3 axis, float rad)
	{
		rad *= 0.5f;
		float a = sin(rad);

		return fquat(
			a * axis.x,
			a * axis.y,
			a * axis.z,
			cos(rad)
		);
	}

	size_t GetAlignedByteSize(size_t size, size_t alignment)
	{
		size_t alignedSize = size / alignment;
		return 16 * (alignedSize + ((size % alignment) > 0));
	}
}