#include "pch.hpp"
#include "D3D11/ShaderVariantManager.hpp"

using namespace Replica;
using namespace Replica::D3D11;

ShaderVariantManager::ShaderVariantManager() :
	pDev(nullptr)
{ }

ShaderVariantManager::ShaderVariantManager(Device& device, const ShaderLibDef& def) :
	pDev(&device),
	libMap(def)
{ }

ShaderVariantManager::ShaderVariantManager(Device& device, ShaderLibDef&& def) :
	pDev(&device),
	libMap(std::move(def))
{ }

const StringIDMap& ShaderVariantManager::GetStringMap() const { return libMap.GetStringMap(); }

bool ShaderVariantManager::TryGetShader(uint shaderID, const VertexShaderVariant*& pVS)
{
	const auto& it = vertexShaders.find(shaderID);

	if (it != vertexShaders.end())
	{
		pVS = &it->second;
		return true;
	}
	else
	{
		const ShaderDefHandle& def = libMap.GetShader(shaderID);

		if (def.GetStage() == ShadeStages::Vertex)
		{
			vertexShaders.emplace(shaderID, VertexShaderVariant(*pDev, def));
			pVS = &vertexShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

bool ShaderVariantManager::TryGetShader(uint shaderID, const PixelShaderVariant*& pPS)
{
	const auto& it = pixelShaders.find(shaderID);

	if (it != pixelShaders.end())
	{
		pPS = &it->second;
		return true;
	}
	else
	{
		const ShaderDefHandle& def = libMap.GetShader(shaderID);

		if (def.GetStage() == ShadeStages::Pixel)
		{
			pixelShaders.emplace(shaderID, PixelShaderVariant(*pDev, def));
			pPS = &pixelShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

bool ShaderVariantManager::TryGetShader(uint shaderID, const ComputeShaderVariant*& pCS)
{
	const auto& it = computeShaders.find(shaderID);

	if (it != computeShaders.end())
	{
		pCS = &it->second;
		return true;
	}
	else
	{
		const ShaderDefHandle& def = libMap.GetShader(shaderID);

		if (def.GetStage() == ShadeStages::Compute)
		{
			computeShaders.emplace(shaderID, ComputeShaderVariant(*pDev, def));
			pCS = &computeShaders[shaderID];
			return true;
		}
		else
			return false;
	}
}

const EffectVariant& ShaderVariantManager::GetEffect(uint effectID)
{
	GFX_ASSERT(effectID != -1, "EffectID out of range");
	const auto& it = effects.find(effectID);

	if (it != effects.end())
		return it->second;
	else
	{
		const EffectDefHandle& def = libMap.GetEffect(effectID);
		effects.emplace(effectID, EffectVariant(*this, def));
		return effects[effectID];
	}
}

const ShaderLibMap& ShaderVariantManager::GetLibMap() const { return libMap; }
