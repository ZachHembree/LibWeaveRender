#pragma once
#include "ShaderLibGen/ShaderData.hpp"
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

	const ShaderLibDef g_BuiltInShaders = 
	{
		.platform = 
		{
			.compilerVersion = "FXC 10.1",
			.shaderModel = "SM 5.0",
			.target = PlatformTargets::DirectX11
		},
		.flagNames = { },
		.modeNames = { },
		.variants = 
		{
			VariantDef
			{
				.effects = 
				{
					{
						.name = "Default",
						.passes =
						{
							EffectPass { .shaderIDs = { 0, 5 } }
						},
						.variantID = 0
					},
					{
						.name = "PosTextured2D",
						.passes =
						{
							EffectPass {.shaderIDs = { 1, 7 } }
						},
						.variantID = 0
					},
					{
						.name = "Textured2D",
						.passes =
						{
							EffectPass {.shaderIDs = { 2, 7 } }
						},
						.variantID = 0
					},
					{
						.name = "DebugFlat3D",
						.passes =
						{
							EffectPass {.shaderIDs = { 3, 6 } }
						},
						.variantID = 0
					},
					{
						.name = "Textured3D",
						.passes =
						{
							EffectPass {.shaderIDs = { 4, 7 } }
						},
						.variantID = 0
					},
				},
				.shaders
				{
					// VS_DefaultSrc - 0
					{
						.fileName = "",
						.binSrc = { (byte*)&VS_DefaultSrc, sizeof(VS_DefaultSrc), },
						.name = "VS_DefaultSrc",
						.stage = ShadeStages::Vertex,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef
						{
							{ "Position", 0u, 0u, DXGI_FORMAT_R32G32_FLOAT, sizeof(vec2) }
						},
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = { },
						.variantID = 0
					},
					// VS_PosTexturedSrc - 1
					{
						.fileName = "",
						.binSrc = { (byte*)&VS_PosTexturedSrc, sizeof(VS_PosTexturedSrc), },
						.name = "VS_PosTexturedSrc",
						.stage = ShadeStages::Vertex,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef
						{
							{ "Position", 0u, 0u, DXGI_FORMAT_R32G32_FLOAT, sizeof(vec2) }
						},
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = { },
						.variantID = 0
					},
					// VS_Textured2D - 2
					{
						.fileName = "",
						.binSrc = { (byte*)&VS_Textured2DSrc, sizeof(VS_Textured2DSrc), },
						.name = "VS_Textured2D",
						.stage = ShadeStages::Vertex,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef
						{
							{ "Position", 0u, 0u, DXGI_FORMAT_R32G32_FLOAT, sizeof(vec2) },
							{ "TexCoord", 0u, 0u, DXGI_FORMAT_R32G32_FLOAT, sizeof(vec2) }
						},
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = { },
						.variantID = 0
					},
					// VS_3D - 3
					{
						.fileName = "",
						.binSrc = { (byte*)&VS_3DSrc, sizeof(VS_3DSrc), },
						.name = "VS_3D",
						.stage = ShadeStages::Vertex,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef
						{
							{ "Position", 0u, 0u, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(vec3) }
						},
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = 
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<mat4>("mvp")
							})
						},
						.variantID = 0
					},
					// VS_Textured3DSrc - 4
					{
						.fileName = "",
						.binSrc = { (byte*)&VS_Textured3DSrc, sizeof(VS_Textured3DSrc), },
						.name = "VS_Textured3DSrc",
						.stage = ShadeStages::Vertex,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef
						{
							{ "Position", 0u, 0u, DXGI_FORMAT_R32G32B32_FLOAT, sizeof(vec3) },
							{ "TexCoord", 0u, 0u, DXGI_FORMAT_R32G32_FLOAT,  sizeof(vec2) }
						},
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = 
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<mat4>("mvp")
							})
						},
						.variantID = 0
					},
					// PS_DefaultSrc - 5
					{
						.fileName = "",
						.binSrc = { (byte*)&PS_DefaultSrc, sizeof(PS_DefaultSrc), },
						.name = "PS_DefaultSrc",
						.stage = ShadeStages::Pixel,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = { },
						.variantID = 0
					},
					// PS_Flat3DSrc - 6
					{
						.fileName = "",
						.binSrc = { (byte*)&PS_Flat3DSrc, sizeof(PS_Flat3DSrc), },
						.name = "PS_Flat3DSrc",
						.stage = ShadeStages::Pixel,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res = { },
						.constBufs = { },
						.variantID = 0
					},
					// PS_TexturedSrc - 7
					{
						.fileName = "",
						.binSrc = { (byte*)&PS_TexturedSrc, sizeof(PS_TexturedSrc), },
						.name = "PS_TexturedSrc",
						.stage = ShadeStages::Pixel,
						.threadGroupSize = tvec3<uint> { },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res =
						{
							ResourceDef { "tex", ShaderTypes::Texture2D },
							ResourceDef { "samp", ShaderTypes::Sampler }
						},
						.constBufs =
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<vec2>("Scale"),
								ConstDef::Get<vec2>("Offset"),
							})
						},
						.variantID = 0
					},
					// Compute Shaders
					{
						.fileName = "",
						.binSrc = { (byte*)&CS_TexCopy2DSrc, sizeof(CS_TexCopy2DSrc) },
						.name = "CS_TexCopy2D",
						.stage = ShadeStages::Compute,
						.threadGroupSize = tvec3<uint> { 1, 1, 1 },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res =
						{
							ResourceDef { "SrcTex", ShaderTypes::Texture2D },
							ResourceDef { "DstTex", ShaderTypes::RWTexture2D }
						},
						.constBufs =
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<ivec2>("SrcOffset"),
								ConstDef::Get<ivec2>("DstOffset")
							})
						},
						.variantID = 0
					},
					{
						.fileName = "",
						.binSrc = { (byte*)&CS_TexCopySamp2DSrc, sizeof(CS_TexCopySamp2DSrc) },
						.name = "CS_TexCopySamp2D",
						.stage = ShadeStages::Compute,
						.threadGroupSize = tvec3<uint> { 1, 1, 1 },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res =
						{
							ResourceDef { "SrcTex", ShaderTypes::Texture2D },
							ResourceDef { "DstTex", ShaderTypes::RWTexture2D },
							ResourceDef { "samp", ShaderTypes::Sampler }
						},
						.constBufs =
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<ivec2>("SrcOffset"),
								ConstDef::Get<ivec2>("DstOffset"),
								ConstDef::Get<vec4>("DstTexelSize")
							})
						},
						.variantID = 0
					},
					{
						.fileName = "",
						.binSrc = { (byte*)&CS_TexCopyScaledSamp2DSrc, sizeof(CS_TexCopyScaledSamp2DSrc) },
						.name = "CS_TexCopyScaledSamp2D",
						.stage = ShadeStages::Compute,
						.threadGroupSize = tvec3<uint> { 1, 1, 1 },
						.inLayout = IOLayoutDef { },
						.outLayout = IOLayoutDef { },
						.res =
						{
							ResourceDef { "SrcTex", ShaderTypes::Texture2D },
							ResourceDef { "DstTex", ShaderTypes::RWTexture2D },
							ResourceDef { "samp", ShaderTypes::Sampler }
						},
						.constBufs =
						{
							ConstBufLayout::GetLayout(
							{
								ConstDef::Get<vec4>("DstTexelSize"),
								ConstDef::Get<vec2>("Scale"),
								ConstDef::Get<vec2>("Offset"),
							})
						},
						.variantID = 0
					}
				}
			}
		}
	};
}
