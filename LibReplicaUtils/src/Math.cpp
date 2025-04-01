#include "pch.hpp"
#include "ReplicaUtils/Math.hpp"

namespace Replica
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
		return (size + alignment - 1) / alignment * alignment;
	}
}