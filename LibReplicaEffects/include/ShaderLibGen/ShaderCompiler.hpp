#pragma once
#define RPFX_D3D11_FEATURE_LEVEL "5_0"
#include "ShaderData.hpp"

namespace Replica::Effects
{ 
	/// <summary>
	/// Precompiles the given HLSL source for D3D11 and generates metadata for resources required by the shader
	/// </summary>
	void GetShaderDefD3D11(string_view srcFile, string_view srcText, ShadeStages stage, string_view mainName, ShaderDef& def);
}