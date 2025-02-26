#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/EffectVariant.hpp"
#include "D3D11/Shaders/ComputeShader.hpp"
#include "D3D11/ShaderSrc/CS_TexCopy2D.hpp"
#include "D3D11/ShaderSrc/CS_TexCopySamp2D.hpp"
#include "D3D11/ShaderSrc/CS_TexCopyScaledSamp2D.hpp"
#include "D3D11/ShaderSrc/PS_Default.hpp"
#include "D3D11/ShaderSrc/PS_Flat3D.hpp"
#include "D3D11/ShaderSrc/PS_Textured.hpp"
#include "D3D11/ShaderSrc/VS_3D.hpp"
#include "D3D11/ShaderSrc/VS_Default.hpp"
#include "D3D11/ShaderSrc/VS_PosTextured.hpp"
#include "D3D11/ShaderSrc/VS_Textured2D.hpp"
#include "D3D11/ShaderSrc/VS_Textured3D.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

const ShaderLibDef s_BuiltInShaders =
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
						EffectPass {.shaderIDs = { 0, 5 } }
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
					.name = "VS_Default",
					.stage = ShadeStages::Vertex,
					.threadGroupSize = tvec3<uint> { },
					.inLayout = IOLayoutDef
					{
						{ "Position", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 2, sizeof(vec2) }
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
					.name = "VS_PosTextured",
					.stage = ShadeStages::Vertex,
					.threadGroupSize = tvec3<uint> { },
					.inLayout = IOLayoutDef
					{
						{ "Position", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 2, sizeof(vec2) }
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
						{ "Position", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 2, sizeof(vec2) },
						{ "TexCoord", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 2, sizeof(vec2)  }
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
						{ "Position", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 3, sizeof(vec3) }
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
					.name = "VS_Textured3D",
					.stage = ShadeStages::Vertex,
					.threadGroupSize = tvec3<uint> { },
					.inLayout = IOLayoutDef
					{
						{ "Position", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 3, sizeof(vec3) },
						{ "TexCoord", 0u, D3D_REGISTER_COMPONENT_FLOAT32, 2, sizeof(vec2) }
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
					.name = "PS_Default",
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
					.name = "PS_Flat3D",
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
					.name = "PS_Textured",
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
					.name = "TexCopy2D",
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
					.name = "TexCopySamp2D",
					.stage = ShadeStages::Compute,
					.threadGroupSize = tvec3<uint> { 1, 1, 1 },
					.inLayout = IOLayoutDef { },
					.outLayout = IOLayoutDef { },
					.res =
					{
						ResourceDef { "SrcTex", ShaderTypes::Texture2D },
						ResourceDef { "DstTex", ShaderTypes::RWTexture2D },
						ResourceDef { "Samp", ShaderTypes::Sampler }
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
					.name = "TexCopyScaledSamp2D",
					.stage = ShadeStages::Compute,
					.threadGroupSize = tvec3<uint> { 1, 1, 1 },
					.inLayout = IOLayoutDef { },
					.outLayout = IOLayoutDef { },
					.res =
					{
						ResourceDef { "SrcTex", ShaderTypes::Texture2D },
						ResourceDef { "DstTex", ShaderTypes::RWTexture2D },
						ResourceDef { "Samp", ShaderTypes::Sampler }
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

const ShaderLibDef& Replica::Effects::GetBuiltInShaders()
{
	return s_BuiltInShaders;
}