#include "pch.hpp"
#include "ShaderLibMap.hpp"
#include "ShaderLibGen/ShaderRegistryMap.hpp"

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
	ShaderLibMap::ShaderLibMap()
	{ }

	ShaderLibMap::ShaderLibMap(const ShaderLibDef& def) : 
		pRegMap(new ShaderRegistryMap(def.regData)),
		platform(def.platform),
		flagIDs(def.flagIDs),
		modeIDs(def.modeIDs),
		variants(def.variants)
	{
		InitMap();
	}

	ShaderLibMap::ShaderLibMap(ShaderLibDef&& def) noexcept : 
		pRegMap(new ShaderRegistryMap(std::move(def.regData))),
		platform(std::move(def.platform)),
		flagIDs(std::move(def.flagIDs)),
		modeIDs(std::move(def.modeIDs)),
		variants(std::move(def.variants))
	{
		InitMap();
	}

	ShaderLibMap::~ShaderLibMap() = default;

	void ShaderLibMap::InitMap()
	{
		// Initialize tables
		// Compile flags
		for (int i = 0; i < flagIDs.GetLength(); i++)
		{
			const uint nameID = flagIDs[i];
			flagNameMap.emplace(nameID, i);
		}
		// Shader modes
		for (int i = 0; i < modeIDs.GetLength(); i++)
		{
			const uint nameID = modeIDs[i];
			modeNameMap.emplace(nameID, i);
		}

		// Variant lookup tables
		variantMaps = UniqueArray<VariantNameMap>(variants.GetLength());

		for (int vID = 0; vID < variants.GetLength(); vID++)
		{
			const auto& variant = variants[vID];
			variantMaps[vID] = VariantNameMap();

			// Shaders
			for (int i = 0; i < variant.shaders.GetLength(); i++)
			{
				const ShaderVariantDef& vidPair = variant.shaders[i];
				const ShaderDef& shader = pRegMap->GetShader(vidPair.shaderID);
				NameIndexMap& map = variantMaps[vID].nameShaderMap;
				map.emplace(shader.nameID, vidPair.shaderID);
			}

			// Effects
			for (int i = 0; i < variant.effects.GetLength(); i++)
			{
				const EffectVariantDef& vidPair = variant.effects[i];
				const EffectDef& effect = pRegMap->GetEffect(vidPair.effectID);
				NameIndexMap& map = variantMaps[vID].effectNameMap;
				map.emplace(effect.nameID, vidPair.effectID);
			}
		}
	}

	const StringIDMap& ShaderLibMap::GetStringMap() const { return pRegMap->GetStringMap(); }

	/// <summary>
	/// Returns the shader by shaderID and variantID
	/// </summary>
	ShaderDefHandle ShaderLibMap::GetShader(uint shaderID) const
	{ 
		REP_ASSERT_MSG(shaderID != -1, "Shader ID invalid");
		return ShaderDefHandle(*pRegMap, shaderID);
	}

	/// <summary>
	/// Returns the effect with the given effectID and variantID
	/// </summary>
	EffectDefHandle ShaderLibMap::GetEffect(uint effectID) const
	{ 
		REP_ASSERT_MSG(effectID != -1, "Effect ID invalid");
		return EffectDefHandle(*pRegMap, effectID);
	}

	uint ShaderLibMap::TryGetShaderID(uint nameID, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].nameShaderMap;
		const auto& it = nameMap.find(nameID);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = GetVariant(vID);
			const uint shaderID = it->second;
			return shaderID;
		}
		else
			return -1;
	}

	uint ShaderLibMap::TryGetEffectID(uint nameID, int vID) const
	{
		const NameIndexMap& nameMap = variantMaps[vID].effectNameMap;
		const auto& it = nameMap.find(nameID);

		if (it != nameMap.end())
		{
			const VariantDef& variantDef = GetVariant(vID);
			const uint effectID = it->second;
			return effectID;
		}
		else
			return -1;
	}

	int ShaderLibMap::TryGetMode(uint nameID) const
	{
		const auto& result = modeNameMap.find(nameID);

		if (result != modeNameMap.end())
			return (int)(result->second);

		return -1;
	}

	size_t ShaderLibMap::GetFlagVariantCount() const { return (1ull << flagIDs.GetLength()); }

	size_t ShaderLibMap::GetModeCount() const { return modeIDs.GetLength(); }

	size_t ShaderLibMap::GetVariantCount() const { return modeIDs.GetLength() * GetFlagVariantCount(); }

	size_t ShaderLibMap::GetShaderCount(int vID) const { return GetVariant(vID).shaders.GetLength(); }

	size_t ShaderLibMap::GetEffectCount(int vID) const { return GetVariant(vID).effects.GetLength(); }

	uint ShaderLibMap::GetFlagID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return (uint)variantID % (uint)GetFlagVariantCount();
	}

	uint ShaderLibMap::GetModeID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return (uint)variantID / (uint)GetFlagVariantCount();
	}

	int ShaderLibMap::GetVariantID(const uint flagID, const uint modeID) const
	{
		PARSE_ASSERT_MSG(flagID >= 0 && modeID >= 0, "Invalid flag or mode ID");
		return (int)flagID + ((int)modeID * (int)GetFlagVariantCount());
	}

	bool ShaderLibMap::GetIsDefined(uint nameID, const int vID) const
	{
		const auto flagIt = flagNameMap.find(nameID);

		if (flagIt != flagNameMap.end())
		{
			const int flags = GetFlagID(vID);	
			return (flags & (1 << flagIt->second)) > 0;
		}
		else
		{
			const auto modeIt = modeNameMap.find(nameID);

			if (modeIt != modeNameMap.end())
			{
				const int mode = GetModeID(vID);
				return mode == modeIt->second;
			}
			else
				return false;
		}
	}

	bool ShaderLibMap::GetIsDefined(string_view name, const int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return GetIsDefined(id, vID);
		else
			return false;
	}

	const IDynamicArray<VariantDef>& ShaderLibMap::GetVariants() const { return variants; }

	const VariantDef& ShaderLibMap::GetVariant(const int vID) const
	{
		REP_ASSERT_MSG(vID >= 0 && vID < variants.GetLength(), "Variant ID invalid");
		return variants[vID];
	}

	int ShaderLibMap::TryGetFlags(const std::initializer_list<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderLibMap::TryGetFlags(const IDynamicArray<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderLibMap::TryGetFlags(const std::initializer_list<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderLibMap::TryGetFlags(const IDynamicArray<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	void ShaderLibMap::GetDefines(const int vID, Vector<uint>& defines) const
	{
		const uint flags = GetFlagID(vID);
		const uint mode = GetModeID(vID);

		if (mode > 0)
			defines.Add(modeIDs[mode]);

		for (uint i = 0; i < flagIDs.GetLength(); i++)
		{
			const uint flag = (1u << i);

			if ((flags & flag) == flag)
				defines.Add(flagIDs[i]);
		}
	}

	void ShaderLibMap::GetDefines(const int vID, Vector<string_view>& defines) const
	{
		const uint flags = GetFlagID(vID);
		const uint mode = GetModeID(vID);

		if (mode > 0)
			defines.Add(GetStringMap().GetString(modeIDs[mode]));

		for (uint i = 0; i < flagIDs.GetLength(); i++)
		{
			const uint flag = (1u << i);

			if ((flags & flag) == flag)
				defines.Add(GetStringMap().GetString(flagIDs[i]));
		}
	}

	int ShaderLibMap::SetFlag(uint nameID, bool value, int vID) const
	{
		const auto& flagIt = flagNameMap.find(nameID);

		if (flagIt != flagNameMap.end())
		{
			const uint flag = flagIt->second;
			const uint mode = GetModeID(vID);
			uint flags = GetFlagID(vID);

			if (value)
				flags |= flag;
			else
				flags &= ~flag;

			return GetVariantID(flags, mode);
		}
		else
			return -1;
	}

	int ShaderLibMap::SetFlag(string_view name, bool value, int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetFlag(id, value, vID);
		else
			return -1;
	}

	int ShaderLibMap::SetMode(uint nameID, int vID) const
	{
		const auto modeIt = modeNameMap.find(nameID);

		if (modeIt != modeNameMap.end())
		{
			const uint mode = modeIt->second;
			const uint flags = GetFlagID(vID);
			return GetVariantID(flags, mode);
		}
		else
			return -1;
	}

	int ShaderLibMap::ResetMode(int vID) const
	{
		const uint flags = GetFlagID(vID);
		return GetVariantID(flags, 0u);
	}

	int ShaderLibMap::SetMode(string_view name, int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetMode(id, vID);
		else
			return -1;
	}
}
