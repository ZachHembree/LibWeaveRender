#include "ShaderLibGen/ShaderLibrary.hpp"

/* Variant ID generation

• Definitions:
	Let { F, M, Vid, Fc, Mc, Fv, Vc } be positive integers.
	F, M, and Vid represent flags, mode, and variant ID, respectively.
	Fc and Mc denote the flag count and mode count, respectively.
	Fv is the flag variant count, where Fv = 2 ^ Fc.
	Vc is the total variant count, where Vc = Fv * Mc.
	Define F = (Vid % Fv) and M = floor(Vid / Fv) <=> Vid = F + (M * Fv).

• Storage Details:

	Flag (F) names and mode (M) names are stored in contiguous arrays, maintaining their original 
	order. This ensures the correct bit order and provides counts Fc and Mc. Using these, Fv can be 
	recalculated to support ID <=> Flag/Mode conversion.

	Fv = 2 ^ Fc
	F(Vid) = (Vid % Fv)
	M(Vid) = floor(Vid / Fv)
	Vid(F, M) = F + (M * Fv)
*/

namespace Replica::Effects
{
	ShaderLibrary::ShaderLibrary() : pDef(nullptr)
	{ }

	ShaderLibrary::ShaderLibrary(const ShaderLibDef* pDef) : pDef(pDef)
	{
		// Initialize tables
		// Compile flags
		for (int i = 0; i < pDef->flagNames.GetLength(); i++)
		{
			const string& name = pDef->flagNames[i];
			flagNameMap.emplace(name, i);
		}
		// Shader modes
		for (int i = 0; i < pDef->modeNames.GetLength(); i++)
		{
			const string& name = pDef->flagNames[i];
			modeNameMap.emplace(name, i);
		}

		// Variant lookup tables
		variantMaps = UniqueArray<VariantNameMap>(pDef->variants.GetLength());

		for (int vID = 0; vID < pDef->variants.GetLength(); vID++)
		{
			const auto& variant = pDef->variants[vID];
			variantMaps[vID] = VariantNameMap();

			// Shaders
			for (int i = 0; i < variant.shaders.GetLength(); i++)
			{
				const ShaderDef& shader = variant.shaders[i];
				NameIndexMap& map = variantMaps[vID].nameShaderMap;
				map.emplace(shader.name, i);
			}

			// Effects
			for (int i = 0; i < variant.effects.GetLength(); i++)
			{
				const EffectDef& effect = variant.effects[i];
				NameIndexMap& map = variantMaps[vID].effectNameMap;
				map.emplace(effect.name, i);
			}
		}
	}
	const ShaderDef* ShaderLibrary::TryGetShader(string_view name, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].nameShaderMap;
		const auto& it = nameMap.find(name);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = pDef->variants[vID];
			const int shaderID = it->second;
			return &variantDef.shaders[shaderID];
		}
		else
			return nullptr;
	}

	const EffectDef* ShaderLibrary::TryGetEffect(string_view name, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].effectNameMap;
		const auto& it = nameMap.find(name);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = pDef->variants[vID];
			const int effectID = it->second;
			return &variantDef.effects[effectID];
		}
		else
			return nullptr;
	}

	int ShaderLibrary::TryGetMode(const string_view name) const
	{
		const auto& result = modeNameMap.find(name);

		if (result != modeNameMap.end())
			return (int)(result->second);

		return -1;
	}

	size_t ShaderLibrary::GetFlagVariantCount() const { return (1ull << pDef->flagNames.GetLength()); }

	size_t ShaderLibrary::GetModeCount() const { return pDef->modeNames.GetLength(); }

	size_t ShaderLibrary::GetVariantCount() const { return pDef->modeNames.GetLength() * GetFlagVariantCount(); }

	int ShaderLibrary::GetFlagID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return variantID % (int)GetFlagVariantCount();
	}

	int ShaderLibrary::GetModeID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return variantID / (int)GetFlagVariantCount();
	}

	int ShaderLibrary::GetVariantID(const int flagID, const int modeID) const
	{
		PARSE_ASSERT_MSG(flagID >= 0 && modeID >= 0, "Invalid flag or mode ID");
		return flagID + (modeID * (int)GetFlagVariantCount());
	}
}
