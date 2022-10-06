#pragma once
#include "../Effect.hpp"
#include "../Shaders/ComputeShader.hpp"
#include "../ShaderSrc/CS_TexCopy2D.hpp"
#include "../ShaderSrc/CS_TexCopySamp2D.hpp"
#include "../ShaderSrc/CS_TexCopyScaledSamp2D.hpp"
#include "../ShaderSrc/PS_Default.hpp"
#include "../ShaderSrc/PS_Flat3D.hpp"
#include "../ShaderSrc/PS_Textured.hpp"
#include "../ShaderSrc/VS_3D.hpp"
#include "../ShaderSrc/VS_Default.hpp"
#include "../ShaderSrc/VS_PosTextured.hpp"
#include "../ShaderSrc/VS_Textured2D.hpp"
#include "../ShaderSrc/VS_Textured3D.hpp"

namespace Replica::D3D11
{
	// Compute Shaders

	const ComputeShaderDef g_CS_TexCopy2D
	{
		{
			L"",
			// Source binary
			(byte*)&CS_TexCopy2DSrc,
			sizeof(CS_TexCopy2DSrc),
			// Constants
			{ 
				ConstantDef::Get<ivec2>("SrcOffset"),
				ConstantDef::Get<ivec2>("DstOffset"),
			},
			// Samplers
			{ },
			// Textures
			{
				"SrcTex",
			},
		},
		// RWTextures
		{
			"DstTex"
		}
	};

	const ComputeShaderDef g_CS_TexCopySamp2D
	{
		{
			L"",
			// Source binary
			(byte*)&CS_TexCopySamp2DSrc,
			sizeof(CS_TexCopySamp2DSrc),
			// Constants
			{
				ConstantDef::Get<ivec2>("SrcOffset"),
				ConstantDef::Get<ivec2>("DstOffset"),
				ConstantDef::Get<vec4>("DstTexelSize"),
			},
			// Samplers
			{
				"Samp",
			},
			// Textures
			{
				"SrcTex",
			},
		},
		// RWTextures
		{
			"DstTex"
		}
	};

	// Compute Shaders
	const ComputeShaderDef g_CS_TexCopyScaledSamp2D
	{
		{
			L"",
			// Source binary
			(byte*)&CS_TexCopyScaledSamp2DSrc,
			sizeof(CS_TexCopyScaledSamp2DSrc),
			// Constants
			{
				ConstantDef::Get<vec4>("DstTexelSize"),
				ConstantDef::Get<vec2>("Scale"),
				ConstantDef::Get<vec2>("Offset"),
			},
			// Samplers
			{
				"Samp",
			},
			// Textures
			{
				"SrcTex",
			},
		},
		// RWTextures
		{
			"DstTex"
		}
	};

	// Vertex Shaders
	const VertexShaderDef g_VS_Default =
	{
		// Source binary
		{ 
			L"",
			// Source binary
			(byte*)&VS_DefaultSrc,
			sizeof(VS_DefaultSrc)
		},
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_PosTextured =
	{
		// Source binary
		{
			L"",
			// Source binary
			(byte*)&VS_PosTexturedSrc,
			sizeof(VS_PosTexturedSrc)
		},
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured2D =
	{
		// Source binary
		{
			L"",
			// Source binary
			(byte*)&VS_Textured2DSrc,
			sizeof(VS_Textured2DSrc)
		},		
		// Vertex Layout
		{
			{ "Position", Formats::R32G32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_3D =
	{
		{
			L"",
			// Source binary
			(byte*)&VS_3DSrc,
			sizeof(VS_3DSrc),
			{ ConstantDef::Get<mat4>("mvp"), }
		},
		// Vertex Layout
		{
			{ "Position", Formats::R32G32B32_FLOAT },
		},
	};

	const VertexShaderDef g_VS_Textured3D =
	{
		{
			L"",
			// Source binary
			(byte*)&VS_Textured3DSrc,
			sizeof(VS_Textured3DSrc),
			// Constants
			{ ConstantDef::Get<mat4>("mvp"), }
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
		L"",
		// Source binary
		(byte*)&PS_DefaultSrc,
		sizeof(PS_DefaultSrc)
	};

	const PixelShaderDef g_PS_Flat3D =
	{
		L"",
		// Source binary
		(byte*)&PS_Flat3DSrc,
		sizeof(PS_Flat3DSrc)
	};

	const PixelShaderDef g_PS_Textured =
	{
		L"",
		// Source binary
		(byte*)&PS_TexturedSrc,
		sizeof(PS_TexturedSrc),
		// Constants
		{ 
			ConstantDef::Get<vec2>("Scale"),
			ConstantDef::Get<vec2>("Offset"),
		},
		// Samplers
		{ "samp" },
		// Textures
		{ "tex" },
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