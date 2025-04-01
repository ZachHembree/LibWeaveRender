#pragma once
#include "ReplicaEffects/ShaderData.hpp"
#include "ShaderRegistryBuilder.hpp"

namespace Replica::Effects
{ 
	/// <summary>
	/// Precompiles the given HLSL source for D3D11 and generates metadata for resources required by the shader
	/// </summary>
	uint GetShaderDefD3D11(
		string_view srcFile, 
		string_view srcText, 
		string_view featureLevel,
		ShadeStages stage, 
		string_view mainName, 
		ShaderRegistryBuilder& builder
	);

	/// <summary>
	/// Returns the name of the compiler used for D3D11
	/// </summary>
	string_view GetCompilerVersionD3D11();
}