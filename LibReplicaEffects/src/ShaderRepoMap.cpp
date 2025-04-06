#include "pch.hpp"
#include "ReplicaEffects/ShaderRepoMap.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderRegistryMap.hpp"

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
	ShaderRepoMap::ShaderRepoMap() : pRegMap(nullptr)
	{ }

	ShaderRepoMap::ShaderRepoMap(const VariantRepoDef& def, const ShaderRegistryMap& reg) :
		pRegMap(&reg),
		repoData(def)
	{
		InitMap();
	}

	ShaderRepoMap::ShaderRepoMap(VariantRepoDef&& def, const ShaderRegistryMap& reg) noexcept :
		pRegMap(&reg),
		repoData(std::move(def))
	{
		InitMap();
	}

	ShaderRepoMap::~ShaderRepoMap() = default;

	void ShaderRepoMap::InitMap()
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

	const StringIDMap& ShaderRepoMap::GetStringMap() const { return pRegMap->GetStringMap(); }

	uint ShaderRepoMap::TryGetShaderID(uint nameID, int vID) const
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

	uint ShaderRepoMap::TryGetEffectID(uint nameID, int vID) const
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

	int ShaderRepoMap::TryGetMode(uint nameID) const
	{
		const auto& result = modeNameMap.find(nameID);

		if (result != modeNameMap.end())
			return (int)(result->second);

		return -1;
	}

	size_t ShaderRepoMap::GetFlagVariantCount() const { return (1ull << repoData.flagIDs.GetLength()); }

	size_t ShaderRepoMap::GetModeCount() const { return repoData.modeIDs.GetLength(); }

	size_t ShaderRepoMap::GetVariantCount() const { return repoData.modeIDs.GetLength() * GetFlagVariantCount(); }

	size_t ShaderRepoMap::GetShaderCount(int vID) const { return GetVariant(vID).shaders.GetLength(); }

	size_t ShaderRepoMap::GetEffectCount(int vID) const { return GetVariant(vID).effects.GetLength(); }

	uint ShaderRepoMap::GetFlagID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return (uint)variantID % (uint)GetFlagVariantCount();
	}

	uint ShaderRepoMap::GetModeID(const int variantID) const
	{
		PARSE_ASSERT_MSG(variantID >= 0, "Invalid variant ID");
		return (uint)variantID / (uint)GetFlagVariantCount();
	}

	int ShaderRepoMap::GetVariantID(const uint flagID, const uint modeID) const
	{
		PARSE_ASSERT_MSG(flagID >= 0 && modeID >= 0, "Invalid flag or mode ID");
		return (int)flagID + ((int)modeID * (int)GetFlagVariantCount());
	}

	bool ShaderRepoMap::GetIsDefined(uint nameID, const int vID) const
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

	bool ShaderRepoMap::GetIsDefined(string_view name, const int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return GetIsDefined(id, vID);
		else
			return false;
	}

	const IDynamicArray<VariantDef>& ShaderRepoMap::GetVariants() const { return repoData.variants; }

	const VariantDef& ShaderRepoMap::GetVariant(const int vID) const
	{
		REP_ASSERT_MSG(vID >= 0 && vID < repoData.variants.GetLength(), "Variant ID invalid");
		return repoData.variants[vID];
	}

	int ShaderRepoMap::TryGetFlags(const std::initializer_list<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderRepoMap::TryGetFlags(const IDynamicArray<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderRepoMap::TryGetFlags(const std::initializer_list<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	int ShaderRepoMap::TryGetFlags(const IDynamicArray<uint>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

	void ShaderRepoMap::GetDefines(const int vID, Vector<uint>& defines) const
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

	void ShaderRepoMap::GetDefines(const int vID, Vector<string_view>& defines) const
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

	int ShaderRepoMap::SetFlag(uint nameID, bool value, int vID) const
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

	int ShaderRepoMap::SetFlag(string_view name, bool value, int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetFlag(id, value, vID);
		else
			return -1;
	}

	int ShaderRepoMap::SetMode(uint nameID, int vID) const
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

	int ShaderRepoMap::ResetMode(int vID) const
	{
		const uint flags = GetFlagID(vID);
		return GetVariantID(flags, 0u);
	}

	int ShaderRepoMap::SetMode(string_view name, int vID) const
	{
		uint id = -1;

		if (GetStringMap().TryGetStringID(name, id))
			return SetMode(id, vID);
		else
			return -1;
	}
}
