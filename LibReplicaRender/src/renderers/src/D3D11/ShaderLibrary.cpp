#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"
#include "D3D11/Shaders/ShaderBase.hpp"
#include "D3D11/Shaders/VertexShader.hpp"
#include "D3D11/Shaders/PixelShader.hpp"
#include "D3D11/EffectVariant.hpp"

using namespace Replica::D3D11;

ShaderLibrary::ShaderLibrary() :
	pDev(nullptr)
{ }

ShaderLibrary::ShaderLibrary(Device& device, const ShaderLibDef& def) :
	pDev(&device),
	libMap(def)
{ }

ShaderLibrary::ShaderLibrary(Device& device, ShaderLibDef&& def) :
	pDev(&device),
	libMap(std::move(def))
{ }

bool ShaderLibrary::TryGetShader(const int shaderID, const VertexShaderVariant*& pVS, const int vID)
{
	Variant& variant = GetVariant(vID);
	const auto& it = variant.vertexShaders.find(shaderID);

	if (it != variant.vertexShaders.end())
	{
		pVS = &it->second;
		return true;
	}
	else
	{
		const ShaderDef& def = libMap.GetShader(shaderID, vID);

		if (def.stage == ShadeStages::Vertex)
		{
			variant.vertexShaders.emplace(shaderID, VertexShaderVariant(*pDev, *this, def));
			pVS = &variant.vertexShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

bool ShaderLibrary::TryGetShader(const int shaderID, const PixelShaderVariant*& pPS, const int vID)
{
	Variant& variant = GetVariant(vID);
	const auto& it = variant.pixelShaders.find(shaderID);

	if (it != variant.pixelShaders.end())
	{
		pPS = &it->second;
		return true;
	}
	else
	{
		const ShaderDef& def = libMap.GetShader(shaderID, vID);

		if (def.stage == ShadeStages::Pixel)
		{
			variant.pixelShaders.emplace(shaderID, PixelShaderVariant(*pDev, *this, def));
			pPS = &variant.pixelShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

bool ShaderLibrary::TryGetShader(const int shaderID, const ComputeShaderVariant*& pCS, const int vID)
{
	Variant& variant = GetVariant(vID);
	const auto& it = variant.computeShaders.find(shaderID);

	if (it != variant.computeShaders.end())
	{
		pCS = &it->second;
		return true;
	}
	else
	{
		const ShaderDef& def = libMap.GetShader(shaderID, vID);

		if (def.stage == ShadeStages::Compute)
		{
			variant.computeShaders.emplace(shaderID, ComputeShaderVariant(*pDev, *this, def));
			pCS = &variant.computeShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

EffectVariant& ShaderLibrary::GetEffect(const int effectID, const int vID)
{
	Variant& variant = GetVariant(vID);
	const int effectCount = (int)libMap.GetEffectCount(vID);

	GFX_ASSERT(effectID >= 0 && effectID < effectCount, "EffectID out of range");
	const auto it = variant.effects.find(effectID);

	if (it != variant.effects.end())
		return it->second;
	else
	{
		const EffectDef& def = libMap.GetEffect(effectID, vID);
		variant.effects.emplace(effectID, EffectVariant(*this, def));
		return variant.effects[effectID];
	}
}

EffectVariant& ShaderLibrary::GetEffect(string_view name, const int vID)
{
	const int effectID = libMap.TryGetEffectID(name, vID);

	if (effectID != -1)
		return GetEffect(effectID, vID);
	else
		GFX_THROW("Effect name invalid");
}

const ShaderLibMap& ShaderLibrary::GetLibMap() const { return libMap; }

ShaderLibrary::Variant& ShaderLibrary::GetVariant(const int vID)
{
	GFX_ASSERT(vID == 0 || (vID > 0 && vID < libMap.GetVariantCount()), "Shader variant configuration invalid")

	// Create variant if not instantiated
	if (!variants.contains(vID))
		variants.emplace(vID, Variant());

	return variants[vID];
}
