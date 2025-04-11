#pragma once
#include "ReplicaEffects/ShaderData.hpp"

namespace Replica::Effects
{
	// Common Vertex Formats
	struct VertexPos3D
	{
		vec3 pos;
	};

	struct VertexPos2D
	{
		vec2 pos;
	};

	struct VertexPosUv3D
	{
		vec3 pos;
		vec2 uv;
	};

	struct VertexPosUv2D
	{
		vec2 pos;
		vec2 uv;
	};

	ShaderLibDef GetBuiltInShaders();
}
