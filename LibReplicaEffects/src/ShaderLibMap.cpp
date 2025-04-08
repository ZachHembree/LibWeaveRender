#include "pch.hpp"
#include "ReplicaEffects/ShaderLibMap.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"

/* Variant ID generation

• Definitions:
	Let { Ri, F, M, Cid, Fc, Mc, Fv, Vc } be positive integers.
	Ri represents the Variant Repository Index.
	F, M, and Cid represent flags, mode, and config ID, respectively.
	Fc and Mc denote the flag count and mode count, respectively.
	Fv is the flag config variant count, where Fv = 2 ^ Fc.
	Vc is the total config variant count, where Vc = Fv * Mc.
	Define F = (Cid % Fv) and M = floor(Cid / Fv) <=> Cid = F + (M * Fv).

• Storage Details:

	Flag (F) names and mode (M) names are stored in contiguous arrays, maintaining their original
	order. This ensures the correct bit order and provides counts Fc and Mc. Using these, Fv can be
	recalculated to support ID <=> Flag/Mode conversion.

	Fv = 2 ^ Fc
	F(Cid) = (Cid % Fv)
	M(Cid) = floor(Cid / Fv)
	Cid(F, M) = F + (M * Fv)

	Vid(Ri, Cid) = (Ri << 16) | (Cid & 0xFFFF)
	Ri(Vid) = Vid >> 16
	Cid(Vid) = Vid & 0xFFFF
*/

using namespace Replica;
using namespace Replica::Effects;

static uint GetConfigFlags(uint configID, uint fvCount) { return configID % fvCount; }

static uint GetConfigMode(uint configID, uint fvCount) { return configID / fvCount; }

static uint GetConfigIndex(uint flagID, uint modeID, uint fvCount) { return flagID + (modeID * fvCount); }

static uint PackVariantID(uint repoIndex, uint configID) { return (repoIndex << g_VariantGroupOffset) | configID; }

static uint GetRepoIndex(uint vID) { return vID >> g_VariantGroupOffset; }

static uint GetConfigIndex(uint vID) { return (vID & g_VariantMask); }

ShaderLibMap::ShaderLibMap() = default;

ShaderLibMap::~ShaderLibMap() = default;

const StringIDMap& ShaderLibMap::GetStringMap() const { return pRegMap->GetStringMap(); }

ShaderLibMap::ShaderLibMap(const ShaderLibDef& def) :
	platform(def.platform),
	variantShaderMaps(def.repos.GetLength()),
	variantFlagMaps(def.repos.GetLength()),
	variantModeMaps(def.repos.GetLength()),
	variantRepos(def.repos),
	pRegMap(new ShaderRegistryMap(def.regData))
{
	InitMaps();
}

ShaderLibMap::ShaderLibMap(ShaderLibDef&& def) :
	platform(std::move(def.platform)),
	variantShaderMaps(def.repos.GetLength()),
	variantFlagMaps(def.repos.GetLength()),
	variantModeMaps(def.repos.GetLength()),
	variantRepos(std::move(def.repos)),
	pRegMap(new ShaderRegistryMap(std::move(def.regData)))
{
	InitMaps();
}

void ShaderLibMap::InitMaps()
{
	const uint groupCount = (uint)variantRepos.GetLength();

	for (uint repoIndex = 0; repoIndex < groupCount; repoIndex++)
	{
		const VariantRepoDef& repoDef = variantRepos[repoIndex];
		// map[repoIndex][nameID] -> flag/modeID
		NameIndexMap& flagNameMap = variantFlagMaps[repoIndex];
		NameIndexMap& modeNameMap = variantModeMaps[repoIndex];

		// Shader flags
		for (uint i = 0; i < repoDef.flagIDs.GetLength(); i++)
		{
			const uint flagID = 1u << i;
			flagNameMap.emplace(repoDef.flagIDs[i], flagID);
		}

		// Shader modes
		for (uint i = 0; i < repoDef.modeIDs.GetLength(); i++)
			modeNameMap.emplace(repoDef.modeIDs[i], i);

		// Default global name -> variant mapping
		// sharedVariantMap[nameID] -> vID
		const VariantDef& baseVariant = repoDef.variants[0];
		const uint baseID = PackVariantID(repoIndex, 0);

		// Shaders
		for (const ShaderVariantDef& varShaderPair : baseVariant.shaders)
		{
			const ShaderDef& shader = pRegMap->GetShader(varShaderPair.shaderID);
			sharedVariantMap.shaders[shader.nameID] = baseID;
		}

		// Effects
		for (const EffectVariantDef& varEffectPair : baseVariant.effects)
		{
			const EffectDef& effect = pRegMap->GetEffect(varEffectPair.effectID);
			sharedVariantMap.effects[effect.nameID] = baseID;
		}

		// vID ~= (repoIndex << 16) | flagID
		// variantMap[repoIndex][flagID][nameID] -> shader/effectID
		UniqueArray<VariantNameMap>& variantMap = variantShaderMaps[repoIndex];
		variantMap = UniqueArray<VariantNameMap>(repoDef.variants.GetLength());

		for (uint flagID = 0; flagID < repoDef.variants.GetLength(); flagID++)
		{
			const VariantDef& variant = repoDef.variants[flagID];
			variantMap[flagID] = VariantNameMap();

			// Shaders
			for (const ShaderVariantDef& varShaderPair : variant.shaders)
			{
				const ShaderDef& shader = pRegMap->GetShader(varShaderPair.shaderID);
				NameIndexMap& map = variantMap[flagID].shaders;
				map.emplace(shader.nameID, varShaderPair.shaderID);
			}

			// Effects
			for (const EffectVariantDef& varEffectPair : variant.effects)
			{
				const EffectDef& effect = pRegMap->GetEffect(varEffectPair.effectID);
				NameIndexMap& map = variantMap[flagID].effects;
				map.emplace(effect.nameID, varEffectPair.effectID);
			}
		}
	}
}

ShaderDefHandle ShaderLibMap::GetShader(uint shaderID) const
{
	REP_ASSERT_MSG(shaderID != -1, "Shader ID invalid");
	return ShaderDefHandle(*pRegMap, shaderID);
}

EffectDefHandle ShaderLibMap::GetEffect(uint effectID) const
{
	REP_ASSERT_MSG(effectID != -1, "Effect ID invalid");
	return EffectDefHandle(*pRegMap, effectID);
}

uint ShaderLibMap::TryGetDefaultShaderVariant(uint nameID) const
{
	REP_ASSERT_MSG(nameID != -1, "Name ID invalid");
	const auto& it = sharedVariantMap.shaders.find(nameID);

	if (it != sharedVariantMap.shaders.end())
		return it->second;
	else
		return -1;
}

uint ShaderLibMap::TryGetDefaultEffectVariant(uint nameID) const
{
	REP_ASSERT_MSG(nameID != -1, "Name ID invalid");
	const auto& it = sharedVariantMap.effects.find(nameID);

	if (it != sharedVariantMap.effects.end())
		return it->second;
	else
		return -1;
}

uint ShaderLibMap::TryGetShaderID(uint nameID, uint vID) const 
{
	REP_ASSERT_MSG(vID != -1 && nameID != -1, "Specified shader invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint cfgIndex = GetConfigIndex(vID);
	REP_ASSERT_MSG(repoIndex >= 0 && cfgIndex >= 0
		&& repoIndex < variantShaderMaps.GetLength() 
		&& cfgIndex < variantShaderMaps[repoIndex].GetLength(),
	"Shader variant ID invalid");

	const NameIndexMap& nameMap = variantShaderMaps[repoIndex][cfgIndex].shaders;
	const auto& it = nameMap.find(nameID);

	if (it != nameMap.end())
		return it->second;
	else
		return -1;
}

uint ShaderLibMap::TryGetEffectID(uint nameID, uint vID) const 
{
	REP_ASSERT_MSG(vID != -1 && nameID != -1, "Specified effect invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint cfgIndex = GetConfigIndex(vID);
	REP_ASSERT_MSG(repoIndex >= 0 && cfgIndex >= 0
		&& repoIndex < variantShaderMaps.GetLength()
		&& cfgIndex < variantShaderMaps[repoIndex].GetLength(),
		"Effect variant ID invalid");

	const NameIndexMap& nameMap = variantShaderMaps[repoIndex][cfgIndex].effects;
	const auto& it = nameMap.find(nameID);

	if (it != nameMap.end())
		return it->second;
	else
		return -1;
}

uint ShaderLibMap::TryGetModeID(uint nameID, uint vID) const 
{
	REP_ASSERT_MSG(vID != -1 && nameID != -1, "Specified mode invalid");
	const uint repoIndex = GetRepoIndex(vID);
	REP_ASSERT_MSG(repoIndex >= 0 && repoIndex < variantShaderMaps.GetLength(), "Mode variant ID invalid");

	const NameIndexMap& modeMap = variantModeMaps[repoIndex];
	const auto& it = modeMap.find(nameID);

	if (it != modeMap.end())
		return it->second;
	else
		return -1;
}

uint ShaderLibMap::TryGetFlags(const std::initializer_list<string_view>& defines, uint vID) const { return TryGetFlags(defines.begin(), defines.end(), vID); }

uint ShaderLibMap::TryGetFlags(const IDynamicArray<string_view>& defines, uint vID) const { return TryGetFlags(defines.begin(), defines.end(), vID); }

uint ShaderLibMap::TryGetFlags(const std::initializer_list<uint>& defines, uint vID) const { return TryGetFlags(defines.begin(), defines.end(), vID); }

uint ShaderLibMap::TryGetFlags(const IDynamicArray<uint>& defines, uint vID) const { return TryGetFlags(defines.begin(), defines.end(), vID); }

uint ShaderLibMap::TryGetFlag(uint nameID, uint vID) const
{
	REP_ASSERT_MSG(vID != -1 && nameID != -1, "Specified flag invalid");
	const uint repoIndex = GetRepoIndex(vID);
	REP_ASSERT_MSG(repoIndex >= 0 && repoIndex < variantShaderMaps.GetLength(), "Flag variant ID invalid");

	const NameIndexMap& flagMap = variantFlagMaps[repoIndex];
	const auto& it = flagMap.find(nameID);

	if (it != flagMap.end())
		return it->second;
	else
		return -1;
}

bool ShaderLibMap::GetIsDefined(uint nameID, uint vID) const 
{
	REP_ASSERT_MSG(nameID != -1, "Name ID invalid");
	REP_ASSERT_MSG(vID != -1, "Variant ID invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint cfgIndex = GetConfigIndex(vID);
	const auto& flagIt = variantFlagMaps[repoIndex].find(nameID);
	
	if (flagIt != variantFlagMaps[repoIndex].end())
	{
		const uint flags = GetConfigFlags(cfgIndex, GetFlagVariantCount(repoIndex));
		const uint flag = flagIt->second;
		return (flags & flag) == flag;
	}
	else
	{
		const uint mode = GetModeIndex(repoIndex, cfgIndex);
		const auto& modeIt = variantModeMaps[repoIndex].find(nameID);

		if (modeIt != variantModeMaps[repoIndex].end())
			return mode == modeIt->second;
		else
			return false;
	}
}

bool ShaderLibMap::GetIsDefined(string_view name, uint vID) const 
{
	uint id = -1;

	if (GetStringMap().TryGetStringID(name, id))
		return GetIsDefined(id, vID);
	else
		return false;
}

void ShaderLibMap::GetDefines(uint vID, Vector<uint>& defines) const
{
	REP_ASSERT_MSG(vID != -1, "Variant ID invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	const uint modeID = GetModeIndex(repoIndex, configIndex);
	const IDynamicArray<uint>& modeIDs = variantRepos[repoIndex].modeIDs;
	const IDynamicArray<uint>& flagIDs = variantRepos[repoIndex].flagIDs;
	uint flagID = GetFlags(repoIndex, configIndex);

	if (modeID != 0)
		defines.emplace_back(modeIDs[modeID]);

	uint flagIndex = 0u;

	while (flagID > 0u)
	{
		if ((flagID & 1u) == 1u)
			defines.emplace_back(flagIDs[flagIndex]);

		flagIndex++;
		flagID >>= 1u;
	}
}

void ShaderLibMap::GetDefines(uint vID, Vector<string_view>& defines) const
{
	REP_ASSERT_MSG(vID != -1, "Variant ID invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	const uint modeID = GetModeIndex(repoIndex, configIndex);
	const IDynamicArray<uint>& modeIDs = variantRepos[repoIndex].modeIDs;
	const IDynamicArray<uint>& flagIDs = variantRepos[repoIndex].flagIDs;
	uint flagID = GetFlags(repoIndex, configIndex);

	if (modeID != 0)
		defines.emplace_back(GetStringMap().GetString(modeIDs[modeID]));

	uint flagIndex = 0u;

	while (flagID > 0u)
	{
		if ((flagID & 1u) == 1u)
			defines.emplace_back(GetStringMap().GetString(flagIDs[flagIndex]));

		flagIndex++;
		flagID >>= 1u;
	}
}

uint ShaderLibMap::SetFlags(uint flags, bool value, uint vID) const 
{
	REP_ASSERT_MSG(vID != -1, "Variant ID invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	const uint fvCount = GetFlagVariantCount(repoIndex);
	const uint configMode = GetConfigMode(configIndex, fvCount);
	uint configFlags = GetConfigFlags(configIndex, fvCount);

	if (value)
		configFlags |= flags;
	else
		configFlags &= ~flags;

	return GetConfigIndex(configFlags, configMode, fvCount);
}

uint ShaderLibMap::SetFlag(string_view name, bool value, uint vID) const 
{
	uint id = -1;

	if (GetStringMap().TryGetStringID(name, id))
		return SetFlags(TryGetFlag(id, vID), value, vID);
	else
		return -1;
}

uint ShaderLibMap::ResetVariant(uint vID) const 
{
	const uint repoIndex = GetRepoIndex(vID);
	return PackVariantID(repoIndex, 0);
}

uint ShaderLibMap::SetMode(uint modeID, uint vID) const 
{
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	const uint fvCount = GetFlagVariantCount(repoIndex);
	const uint flagID = GetConfigFlags(configIndex, fvCount);
	return PackVariantID(repoIndex, GetConfigIndex(flagID, modeID, fvCount));
}

uint ShaderLibMap::ResetMode(uint vID) const { return SetMode(0, vID); }

uint ShaderLibMap::SetMode(string_view name, uint vID) const 
{
	uint id = -1;

	if (GetStringMap().TryGetStringID(name, id))
		return SetMode(TryGetModeID(id, vID), vID);
	else
		return -1;
}

uint ShaderLibMap::GetShaderCount(uint vID) const 
{
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	return (uint)variantRepos[repoIndex].variants[configIndex].shaders.GetLength();
}

uint ShaderLibMap::GetEffectCount(uint vID) const 
{
	const uint repoIndex = GetRepoIndex(vID);
	const uint configIndex = GetConfigIndex(vID);
	return (uint)variantRepos[repoIndex].variants[configIndex].effects.GetLength();
}

uint ShaderLibMap::GetFlagVariantCount(uint repoIndex) const
{
	const uint flagCount = (uint)variantRepos[repoIndex].flagIDs.GetLength();
	return 1u << flagCount;
}

uint ShaderLibMap::GetModeCount(uint repoIndex) const
{
	const uint modeCount = (uint)variantRepos[repoIndex].modeIDs.GetLength();
	return modeCount;
}

uint ShaderLibMap::GetFlags(uint repoIndex, uint configIndex) const
{
	return GetConfigFlags(configIndex, GetFlagVariantCount(repoIndex));
}

uint ShaderLibMap::GetModeIndex(uint repoIndex, uint configIndex) const
{
	return GetConfigMode(configIndex, GetFlagVariantCount(repoIndex));
}

uint ShaderLibMap::GetVariantID(uint repoIndex, uint flagID, uint modeID) const
{
	return PackVariantID(repoIndex, GetConfigIndex(flagID, modeID, GetFlagVariantCount(repoIndex)));
}
