#pragma once
#include "D3D11/Effect.hpp"
#include "D3D11/Shaders/ComputeShader.hpp"

namespace Replica::D3D11
{
	// Compute Shaders
	const ComputeShaderDef g_CS_TexCopySamp2D
	{
		{
			L"Shaders/CS_TexCopySamp2D.cso",
			{
				ConstantDef::Get<vec4>(L"DstTexelSize"),
			},
			{
				L"Samp",
			},
			{
				L"SrcTex",
			},
		},
		{
			L"DstTex"
		}
	};

	const ComputeShaderDef g_CS_TexCopy2D
	{
		{
			L"Shaders/CS_TexCopy2D.cso",
			{ },
			{ },
			{
				L"SrcTex",
			},
		},
		{
			L"DstTex"
		}
	};

	// Vertex Shaders
	const VertexShaderDef g_VS_Default =
	{
		{ L"Shaders/VS_Default.cso" },
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_PosTextured =
	{
		{ L"Shaders/VS_PosTextured.cso" },
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured2D =
	{
		{ L"Shaders/VS_Textured2D.cso" },
		{
			{ "Position", Formats::R32G32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_3D =
	{
		{
			L"Shaders/VS_3D.cso",
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		{
			{ "Position", Formats::R32G32B32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured3D =
	{
		{
			L"Shaders/VS_Textured3D.cso",
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		{
			{ "Position", Formats::R32G32B32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	// Pixel Shaders
	const PixelShaderDef g_PS_Default =
	{
		L"Shaders/PS_Default.cso",
	};

	const PixelShaderDef g_PS_Flat3D =
	{
		L"Shaders/PS_Flat3D.cso",
	};

	const PixelShaderDef g_PS_Textured =
	{
		L"Shaders/PS_Textured.cso",
		{ },
		{ L"samp" },
		{ L"tex" },
	};

	// Effects
	const EffectDef g_DefaultEffect =
	{
		g_VS_Default,
		g_PS_Default
	};

	const EffectDef g_PosTextured2DEffect
	{
		g_VS_PosTextured,
		g_PS_Textured
	};

	const EffectDef g_Textured2DEffect
	{
		g_VS_Textured2D,
		g_PS_Textured
	};

	const EffectDef g_DebugFlat3DEffect
	{
		g_VS_3D,
		g_PS_Flat3D
	};

	const EffectDef g_Textured3DEffect
	{
		g_VS_Textured3D,
		g_PS_Textured
	};

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
}