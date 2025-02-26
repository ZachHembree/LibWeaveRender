#include "pch.hpp"
#include "ShaderLibGen/ShaderLibMap.hpp"

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
	ShaderLibMap::ShaderLibMap() : pDef(nullptr)
	{ }

	ShaderLibMap::ShaderLibMap(const ShaderLibDef& def) : pDef(new ShaderLibDef(def))
	{
		InitMap();
	}

	ShaderLibMap::ShaderLibMap(ShaderLibDef&& def) : pDef(std::move(&def))
	{
		InitMap();
	}

	void ShaderLibMap::InitMap()
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

	/// <summary>
	/// Returns the shader by shaderID and variantID
	/// </summary>
	const ShaderDef& ShaderLibMap::GetShader(int shaderID, int vID) const 
	{ 
		REP_ASSERT_MSG(shaderID >= 0 && shaderID < GetVariant(vID).shaders.GetLength(), "Shader ID invalid");
		return GetVariant(vID).shaders[shaderID];
	}

	/// <summary>
	/// Returns the effect with the given effectID and variantID
	/// </summary>
	const EffectDef& ShaderLibMap::GetEffect(int effectID, int vID) const 
	{ 
		REP_ASSERT_MSG(effectID >= 0 && effectID < GetVariant(vID).effects.GetLength(), "Effect ID invalid");
		return GetVariant(vID).effects[effectID];
	}

	int ShaderLibMap::TryGetShaderID(string_view name, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].nameShaderMap;
		const auto& it = nameMap.find(name);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = GetVariant(vID);
			const int shaderID = it->second;
			return shaderID;
		}
		else
			return -1;
	}

	int ShaderLibMap::TryGetEffectID(string_view name, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].effectNameMap;
		const auto& it = nameMap.find(name);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = GetVariant(vID);
			const int effectID = it->second;
			return effectID;
		}
		else
			return -1;
	}

	int ShaderLibMap::TryGetMode(const string_view name) const
	{
		const auto& result = modeNameMap.find(name);

		if (result != modeNameMap.end())
			return (int)(result->second);

		return -1;
	}

	size_t ShaderLibMap::GetFlagVariantCount() const { return (1ull << pDef->flagNames.GetLength()); }

	size_t ShaderLibMap::GetModeCount() const { return pDef->modeNames.GetLength(); }

	size_t ShaderLibMap::GetVariantCount() const { return pDef->modeNames.GetLength() * GetFlagVariantCount(); }

	size_t ShaderLibMap::GetShaderCount(int vID) const { return GetVariant(vID).shaders.GetLength(); }

	size_t ShaderLibMap::GetEffectCount(int vID) const { return GetVariant(vID).effects.GetLength(); }

	int ShaderLibMap::GetFlagID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return variantID % (int)GetFlagVariantCount();
	}

	int ShaderLibMap::GetModeID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return variantID / (int)GetFlagVariantCount();
	}

	int ShaderLibMap::GetVariantID(const int flagID, const int modeID) const
	{
		PARSE_ASSERT_MSG(flagID >= 0 && modeID >= 0, "Invalid flag or mode ID");
		return flagID + (modeID * (int)GetFlagVariantCount());
	}

	bool ShaderLibMap::GetIsDefined(string_view name, const int vID) const
	{
		const auto flagIt = flagNameMap.find(name);

		if (flagIt != flagNameMap.end())
		{
			const int flags = GetFlagID(vID);	
			return (flags & (1 << flagIt->second)) > 0;
		}
		else
		{
			const auto modeIt = modeNameMap.find(name);

			if (modeIt != modeNameMap.end())
			{
				const int mode = GetModeID(vID);
				return mode == modeIt->second;
			}
			else
				return false;
		}
	}

	const VariantDef& ShaderLibMap::GetVariant(const int vID) const
	{
		REP_ASSERT_MSG(vID >= 0 && vID < pDef->variants.GetLength(), "Variant ID invalid");
		return pDef->variants[vID];
	}
}
