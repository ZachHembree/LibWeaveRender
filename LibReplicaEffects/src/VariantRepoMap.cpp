#include "pch.hpp"
#include "ReplicaEffects/VariantRepoMap.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"

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
	VariantRepoMap::VariantRepoMap() : pRegMap(nullptr)
	{ }

	VariantRepoMap::VariantRepoMap(const VariantRepoDef& def, const ShaderRegistryMap& reg) :
		pRegMap(&reg),
		repoData(def)
	{
		InitMap();
	}

	VariantRepoMap::VariantRepoMap(VariantRepoDef&& def, const ShaderRegistryMap& reg) noexcept :
		pRegMap(&reg),
		repoData(std::move(def))
	{
		InitMap();
	}

	VariantRepoMap::~VariantRepoMap() = default;

	void VariantRepoMap::InitMap()
	{
		// Initialize tables
		// Compile flags
		for (int i = 0; i < repoData.flagIDs.GetLength(); i++)
		{
			const uint nameID = repoData.flagIDs[i];
			flagNameMap.emplace(nameID, i);
		}
		// Shader modes
		for (int i = 0; i < repoData.modeIDs.GetLength(); i++)
		{
			const uint nameID = repoData.modeIDs[i];
			modeNameMap.emplace(nameID, i);
		}

		// Variant lookup tables
		variantMaps = UniqueArray<VariantNameMap>(repoData.variants.GetLength());

		for (int vID = 0; vID < repoData.variants.GetLength(); vID++)
		{
			const VariantDef& variant = repoData.variants[vID];
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

	const StringIDMap& VariantRepoMap::GetStringMap() const { return pRegMap->GetStringMap(); }

	uint VariantRepoMap::TryGetShaderID(uint nameID, uint vID) const
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

	uint VariantRepoMap::TryGetEffectID(uint nameID, uint vID) const
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

	uint VariantRepoMap::TryGetMode(uint nameID) const
	{
		const auto& result = modeNameMap.find(nameID);

		if (result != modeNameMap.end())
			return result->second;

		return -1;
	}

	uint VariantRepoMap::GetFlagVariantCount() const { return (1u << (uint)repoData.flagIDs.GetLength()); }

	uint VariantRepoMap::GetModeCount() const { return (uint)repoData.modeIDs.GetLength(); }

	uint VariantRepoMap::GetVariantCount() const { return (uint)repoData.modeIDs.GetLength() * GetFlagVariantCount(); }

	uint VariantRepoMap::GetShaderCount(uint vID) const { return (uint)GetVariant(vID).shaders.GetLength(); }

	uint VariantRepoMap::GetEffectCount(uint vID) const { return (uint)GetVariant(vID).effects.GetLength(); }

	uint VariantRepoMap::GetFlagID(uint variantID) const
	{
		PARSE_ASSERT_MSG(variantID != -1, "Invalid variant ID");
		return variantID % GetFlagVariantCount();
	}

	uint VariantRepoMap::GetModeID(uint variantID) const
	{
		PARSE_ASSERT_MSG(variantID != -1, "Invalid variant ID");
		return variantID / GetFlagVariantCount();
	}

	uint VariantRepoMap::GetVariantID(uint flagID, uint modeID) const
	{
		PARSE_ASSERT_MSG(flagID != -1 && modeID != -1, "Invalid flag or mode ID");
		return flagID + (modeID * GetFlagVariantCount());
	}

	bool VariantRepoMap::GetIsDefined(uint nameID, uint vID) const
	{
		const auto& flagIt = flagNameMap.find(nameID);

		if (flagIt != flagNameMap.end())
		{
			const uint flags = GetFlagID(vID);
			return (flags & (1 << flagIt->second)) > 0;
		}
		else
		{
			const auto& modeIt = modeNameMap.find(nameID);

			if (modeIt != modeNameMap.end())
			{
				const uint mode = GetModeID(vID);
				return mode == modeIt->second;
			}
			else
				return false;
		}
	}

	bool VariantRepoMap::GetIsDefined(string_view name, uint vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return GetIsDefined(id, vID);
		else
			return false;
	}

	const IDynamicArray<VariantDef>& VariantRepoMap::GetVariants() const { return repoData.variants; }

	const VariantDef& VariantRepoMap::GetVariant(uint vID) const
	{
		REP_ASSERT_MSG(vID != -1 && vID < repoData.variants.GetLength(), "Variant ID invalid");
		return repoData.variants[vID];
	}

	uint VariantRepoMap::TryGetFlags(const std::initializer_list<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	uint VariantRepoMap::TryGetFlags(const IDynamicArray<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	uint VariantRepoMap::TryGetFlags(const std::initializer_list<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	uint VariantRepoMap::TryGetFlags(const IDynamicArray<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	void VariantRepoMap::GetDefines(uint vID, Vector<uint>& defines) const
	{
		const uint flags = GetFlagID(vID);
		const uint mode = GetModeID(vID);

		if (mode > 0)
			defines.Add(repoData.modeIDs[mode]);

		for (uint i = 0; i < repoData.flagIDs.GetLength(); i++)
		{
			const uint flag = (1u << i);

			if ((flags & flag) == flag)
				defines.Add(repoData.flagIDs[i]);
		}
	}

	void VariantRepoMap::GetDefines(uint vID, Vector<string_view>& defines) const
	{
		const uint flags = GetFlagID(vID);
		const uint mode = GetModeID(vID);

		if (mode > 0)
			defines.Add(GetStringMap().GetString(repoData.modeIDs[mode]));

		for (uint i = 0; i < repoData.flagIDs.GetLength(); i++)
		{
			const uint flag = (1u << i);

			if ((flags & flag) == flag)
				defines.Add(GetStringMap().GetString(repoData.flagIDs[i]));
		}
	}

	uint VariantRepoMap::SetFlag(uint nameID, bool value, uint vID) const
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

	uint VariantRepoMap::SetFlag(string_view name, bool value, uint vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetFlag(id, value, vID);
		else
			return -1;
	}

	uint VariantRepoMap::SetMode(uint nameID, uint vID) const
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

	uint VariantRepoMap::ResetMode(uint vID) const
	{
		const uint flags = GetFlagID(vID);
		return GetVariantID(flags, 0u);
	}

	uint VariantRepoMap::SetMode(string_view name, uint vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetMode(id, vID);
		else
			return -1;
	}
}
