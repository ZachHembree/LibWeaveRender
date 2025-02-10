#pragma once
#include "Shaders/VertexShader.hpp"
#include "Shaders/PixelShader.hpp"
#include "Shaders/ComputeShader.hpp"
#include "ShaderLibGen/ShaderLibMap.hpp"

namespace Replica::D3D11
{
	using Effects::ShaderLibDef;
	using Effects::ShaderLibMap;
	using Effects::ShadeStages;
	using Effects::EffectDef;

	class ShaderLibrary
	{
	public:
		ShaderLibrary() :
			pDev(nullptr)
		{ }

		ShaderLibrary(Device& device, const ShaderLibDef& def) :
			pDev(&device),
			libDef(def),
			libMap(&libDef)
		{ }

		ShaderLibrary(Device& device, ShaderLibDef&& def) :
			pDev(&device),
			libDef(std::move(def)),
			libMap(&libDef)
		{ }

		ShaderBase& GetShader(int shaderID, int vID = 0)
		{
			Variant& variant = GetVariant(vID);
			const int shaderCount = libDef.variants[vID].shaders.GetLength();

			if (variant.shaders.GetLength() == 0)
				variant.shaders = UniqueArray<ShaderBase*>(shaderCount, nullptr);

			GFX_ASSERT(shaderID >= 0 && shaderID < variant.shaders.GetLength(), "ShaderID out of range");

			if (variant.shaders[shaderID] == nullptr)
			{
				const ShaderDef& def = libMap.GetShader(shaderID, vID);
				ShaderBase* pShader = nullptr;

				switch (def.stage)
				{
				case ShadeStages::Vertex:
					pShader = &vertexShaders.emplace_back(*pDev, def);
					break;
				case ShadeStages::Pixel:
					pShader = &pixelShaders.emplace_back(*pDev, def);
					break;
				case ShadeStages::Compute:
					pShader = &computeShaders.emplace_back(*pDev, def);
					break;
				}

				variant.shaders[shaderID] = pShader;
			}

			return *variant.shaders[shaderID];
		}

		Effect& GetEffect(int effectID, int vID = 0)
		{
			Variant& variant = GetVariant(vID);
			const int effectCount = libDef.variants[vID].effects.GetLength();

			if (variant.effects.GetLength() != effectCount)
				variant.effects = UniqueArray<Effect*>(effectCount, nullptr);

			GFX_ASSERT(effectID >= 0 && effectID < variant.effects.GetLength(), "EffectID out of range");

			if (variant.effects[effectID] == nullptr)
			{
				const EffectDef& def = libMap.GetEffect(effectID, vID);
				variant.effects[effectID] = &effects.emplace_back(*this, def);
			}
			
			return *variant.effects[effectID];
		}

		ShaderBase& GetShader(string_view name, int vID = 0)
		{
			const int shaderID = libMap.TryGetShaderID(name, vID);

			if (shaderID != -1)
				return GetShader(shaderID, vID);
			else
				GFX_THROW("Shader name invalid");
		}

		Effect& GetEffect(string_view name, int vID = 0)
		{
			const int effectID = libMap.TryGetEffectID(name, vID);

			if (effectID != -1)
				return GetEffect(effectID, vID);
			else
				GFX_THROW("Effect name invalid");
		}

	private:
		struct Variant
		{		
			UniqueArray<ShaderBase*> shaders;
			UniqueArray<Effect*> effects;		
		};

		UniqueVector<VertexShader> vertexShaders;
		UniqueVector<PixelShader> pixelShaders;
		UniqueVector<ComputeShader> computeShaders;
		UniqueVector<Effect> effects;

		std::unordered_map<int, Variant> variants;
		ShaderLibDef libDef;
		ShaderLibMap libMap;
		Device* pDev;

		Variant& GetVariant(const int vID)
		{
			GFX_ASSERT(vID >= 0 && vID < libDef.variants.GetLength(), "Shader variant configuration invalid")

			// Create variant if not instantiated
			if (!variants.contains(vID))
				variants.emplace(vID, Variant());

			return variants[vID];
		}
	};
}