#pragma once
#include "D3D11/Effect.hpp"

namespace Replica::D3D11
{
	// Vertex Shaders
	const VertexShaderDef g_DefaultVS =
	{
		{ },
		L"Shaders/DefaultVS.cso",
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_PosTexturedVS =
	{
		{ },
		L"Shaders/PosTexturedVS.cso",
		{
			{ "Position", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_Texture2DVS =
	{
		{ },
		L"Shaders/Texture2DVS.cso",
		{
			{ "Position", Formats::R32G32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	const VertexShaderDef g_3DVS =
	{
		{
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		L"Shaders/3DVS.cso",
		{
			{ "Position", Formats::R32G32B32_FLOAT },
		},
	};

	const VertexShaderDef g_Texture3DVS =
	{
		{
			{ ConstantDef::Get<mat4>(L"mvp"), }
		},
		L"Shaders/Texture3DVS.cso",
		{
			{ "Position", Formats::R32G32B32_FLOAT },
			{ "TexCoord", Formats::R32G32_FLOAT },
		},
	};

	// Pixel Shaders
	const PixelShaderDef g_DefaultPS =
	{
		{ },
		L"Shaders/DefaultPS.cso",
	};

	const PixelShaderDef g_Flat3DPS =
	{
		{ },
		L"Shaders/Flat3DPS.cso",
	};

	const PixelShaderDef g_TexturePS =
	{
		{ 
			{ },
			{ L"samp" },
			{ L"tex" },
		},
		L"Shaders/TexturePS.cso",
	};

	// Effects
	const EffectDef g_DefaultEffect =
	{
		g_DefaultVS,
		g_DefaultPS
	};

	const EffectDef g_PosTextured2DEffect
	{
		g_PosTexturedVS,
		g_TexturePS
	};

	const EffectDef g_Textured2DEffect
	{
		g_Texture2DVS,
		g_TexturePS
	};

	const EffectDef g_TexturedQuadEffect
	{
		g_Texture2DVS,
		g_TexturePS
	};

	const EffectDef g_DebugFlat3DEffect
	{
		g_3DVS,
		g_Flat3DPS
	};

	const EffectDef g_Textured3DEffect
	{
		g_Texture3DVS,
		g_TexturePS
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