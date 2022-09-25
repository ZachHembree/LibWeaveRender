#pragma once
#include "../Effect.hpp"
#include "../Shaders/ComputeShader.hpp"

namespace Replica::D3D11
{
	// Compute Shaders

	const ComputeShaderDef g_CS_TexCopy2D
	{
		{
			// Source binary
			L"Shaders/CS_TexCopy2D.cso",
			// Constants
			{ 
				ConstantDef::Get<ivec2>(L"SrcOffset"),
				ConstantDef::Get<ivec2>(L"DstOffset"),
			},
			// Samplers
			{ },
			// Textures
			{
				L"SrcTex",
			},
		},
		// RWTextures
		{
			L"DstTex"
		}
	};

	const ComputeShaderDef g_CS_TexCopySamp2D
	{
		{
			// Source binary
			L"Shaders/CS_TexCopySamp2D.cso",
			// Constants
			{
				ConstantDef::Get<ivec2>(L"SrcOffset"),
				ConstantDef::Get<ivec2>(L"DstOffset"),
				ConstantDef::Get<vec4>(L"DstTexelSize"),
			},
			// Samplers
			{
				L"Samp",
			},
			// Textures
			{
				L"SrcTex",
			},
		},
		// RWTextures
		{
			L"DstTex"
		}
	};

	// Compute Shaders
	const ComputeShaderDef g_CS_TexCopyScaledSamp2D
	{
		{
			// Source binary
			L"Shaders/CS_TexCopyScaledSamp2D.cso",
			// Constants
			{
				ConstantDef::Get<vec4>(L"DstTexelSize"),
				ConstantDef::Get<vec2>(L"Scale"),
				ConstantDef::Get<vec2>(L"Offset"),
			},
			// Samplers
			{
				L"Samp",
			},
			// Textures
			{
				L"SrcTex",
			},
		},
		// RWTextures
		{
			L"DstTex"
		}
	};

	// Vertex Shaders
	const VertexShaderDef g_VS_Default =
	{
		// Source binary
		{ L"Shaders/VS_Default.cso" },
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_PosTextured =
	{
		// Source binary
		{ L"Shaders/VS_PosTextured.cso" },
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured2D =
	{
		// Source binary
		{ L"Shaders/VS_Textured2D.cso" },
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_3D =
	{
		{
			// Source binary
			{ L"Shaders/VS_3D.cso" },
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		// Vertex Layout
		{
			{ "Position", Formats::R32G32B32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured3D =
	{
		{
			// Source binary
			{ L"Shaders/VS_Textured3D.cso" },
			// Constants
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		// Vertex Layout
		{
			{ "Position", Formats::R32G32B32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	// Pixel Shaders
	const PixelShaderDef g_PS_Default =
	{
		// Source binary
		L"Shaders/PS_Default.cso",
	};

	const PixelShaderDef g_PS_Flat3D =
	{
		// Source binary
		L"Shaders/PS_Flat3D.cso",
	};

	const PixelShaderDef g_PS_Textured =
	{
		// Source binary
		L"Shaders/PS_Textured.cso",
		// Constants
		{ 
			ConstantDef::Get<vec2>(L"Scale"),
			ConstantDef::Get<vec2>(L"Offset"),
		},
		// Samplers
		{ L"samp" },
		// Textures
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