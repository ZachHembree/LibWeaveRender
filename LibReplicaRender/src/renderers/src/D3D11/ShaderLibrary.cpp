#include "pch.hpp"
#include "ShaderLibGen/ShaderRegistryMap.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"

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

Material ShaderLibrary::GetMaterial(uint effectNameID) { return Material(*this, effectNameID, 0); }

ComputeInstance ShaderLibrary::GetComputeInstance(uint nameID) { return ComputeInstance(*this, nameID, 0); }

Material ShaderLibrary::GetMaterial(string_view effectName) 
{ 
	uint id = -1;

	if (GetStringMap().TryGetStringID(effectName, id))
		return GetMaterial(id);
	else
		GFX_THROW("Invalid effect name")
}

ComputeInstance ShaderLibrary::GetComputeInstance(string_view name) 
{
	uint id = -1;

	if (GetStringMap().TryGetStringID(name, id))
		return GetComputeInstance(id);
	else
		GFX_THROW("Invalid shader name")
}

bool ShaderLibrary::TryGetShader(uint shaderID, const VertexShaderVariant*& pVS)
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

bool ShaderLibrary::TryGetShader(uint shaderID, const PixelShaderVariant*& pPS)
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

bool ShaderLibrary::TryGetShader(uint shaderID, const ComputeShaderVariant*& pCS)
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

const EffectVariant& ShaderLibrary::GetEffect(uint effectID)
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

const ShaderLibMap& ShaderLibrary::GetLibMap() const { return libMap; }
