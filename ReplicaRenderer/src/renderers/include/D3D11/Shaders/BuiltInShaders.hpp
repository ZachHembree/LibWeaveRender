#pragma once
#include "D3D11/Effect.hpp"

namespace Replica::D3D11
{
	const VertexShaderDef g_DefaultVS =
	{
		{
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		L"DefaultVertShader.cso",
		{
			{ "Position", Formats::R32G32B32_FLOAT },
		},
	};

	const PixelShaderDef g_DefaultPS =
	{
		{
			{ ConstantDef::Get<vec4>(L"DstTexelSize"), },
			{ L"samp" },
			{ L"tex" },
		},
		L"DefaultPixShader.cso",
	};

	const EffectDef g_DefaultEffect =
	{
		g_DefaultVS,
		g_DefaultPS
	};
}