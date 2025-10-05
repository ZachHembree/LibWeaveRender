#include "pch.hpp"
#include "WeaveEffects/ShaderLibMap.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"

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

using namespace Weave;
using namespace Weave::Effects;

static uint PackVariantID(uint repoID, uint configID) { return (repoID << g_VariantGroupOffset) | configID; }

static uint GetRepoIndex(uint vID) { return vID >> g_VariantGroupOffset; }

static uint GetConfigIndex(uint vID) { return (vID & g_VariantMask); }

ShaderLibMap::ShaderLibMap() = default;

ShaderLibMap::~ShaderLibMap() = default;

string_view ShaderLibMap::GetName() const { return name; }

const IStringIDMap& ShaderLibMap::GetStringMap() const { return pRegMap->GetStringMap(); }

ShaderLibMap::ShaderLibMap(const ShaderLibDef::Handle& def) :
	name(*def.pName),
	platform(*def.pPlatform),
	variantShaderMaps(def.pRepos->GetLength()),
	repoConfigTables(def.pRepos->GetLength()),
	variantRepos(*def.pRepos),
	pRegMap(new ShaderRegistryMap(def.regHandle, def.strMapHandle))
{
	InitMaps();
}

ShaderLibMap::ShaderLibMap(ShaderLibDef&& def) :
	name(std::move(def.name)),
	platform(std::move(def.platform)),
	variantShaderMaps(def.repos.GetLength()),
	repoConfigTables(def.repos.GetLength()),
	variantRepos(std::move(def.repos)),
	pRegMap(new ShaderRegistryMap(std::move(def.regData), std::move(def.stringIDs)))
{
	InitMaps();
}

ShaderLibMap::ShaderLibMap(const ShaderLibDef::Handle& def, StringIDBuilder& sharedStringIDs) :
	name(*def.pName),
	platform(*def.pPlatform),
	variantShaderMaps(def.pRepos->GetLength()),
	repoConfigTables(def.pRepos->GetLength()),
	variantRepos(*def.pRepos),
	pRegMap(new ShaderRegistryMap(def.regHandle, def.strMapHandle, sharedStringIDs))
{
	InitMaps();
}

ShaderLibMap::ShaderLibMap(ShaderLibDef&& def, StringIDBuilder& sharedStringIDs) :
	name(std::move(def.name)),
	platform(std::move(def.platform)),
	variantShaderMaps(def.repos.GetLength()),
	repoConfigTables(def.repos.GetLength()),
	variantRepos(std::move(def.repos)),
	pRegMap(new ShaderRegistryMap(std::move(def.regData), def.stringIDs.GetHandle(), sharedStringIDs))
{
	InitMaps();
}

void ShaderLibMap::InitMaps()
{
	const uint groupCount = (uint)variantRepos.GetLength();

	for (uint repoIndex = 0; repoIndex < groupCount; repoIndex++)
	{
		VariantRepoDef& repoDef = variantRepos[repoIndex];
		repoConfigTables[repoIndex] = ConfigIDTable(repoDef.configTable, pRegMap->GetStringMap());

		// Default global name -> variant mapping
		// sharedVariantMap[nameID] -> repoID / vID
		const VariantDef& baseVariant = repoDef.variants[0];
		const uint repoID = repoIndex << g_VariantGroupOffset;

		// Shaders
		for (const ShaderVariantDef& varShaderPair : baseVariant.shaders)
		{
			const ShaderDef& shader = pRegMap->GetShader(varShaderPair.shaderID);
			sharedVariantMap.shaders[shader.nameID] = repoID;
		}

		// Effects
		for (const EffectVariantDef& varEffectPair : baseVariant.effects)
		{
			const EffectDef& effect = pRegMap->GetEffect(varEffectPair.effectID);
			sharedVariantMap.effects[effect.nameID] = repoID;
		}

		// vID ~= (repoIndex << 16) | configID
		// variantMap[repoID][configID][nameID] -> shader/effectID
		UniqueArray<VariantNameMap>& variantMap = variantShaderMaps[repoIndex];
		variantMap = UniqueArray<VariantNameMap>(repoDef.variants.GetLength());

		for (uint configID = 0; configID < repoDef.variants.GetLength(); configID++)
		{
			const VariantDef& variant = repoDef.variants[configID];
			variantMap[configID] = VariantNameMap();

			// Shaders
			for (const ShaderVariantDef& pair : variant.shaders)
			{
				const ShaderDef& shader = pRegMap->GetShader(pair.shaderID);
				// Map shaderIDs to names for each configuration
				variantMap[configID].shaders[shader.nameID] = pair.shaderID;
			}

			// Effects
			for (const EffectVariantDef& pair : variant.effects)
			{
				const EffectDef& effect = pRegMap->GetEffect(pair.effectID);
				// Map effectIDs to names for each configuration
				variantMap[configID].effects[effect.nameID] = pair.effectID;
			}
		}
	}
}

ShaderDefHandle ShaderLibMap::GetShader(uint shaderID) const
{
	FX_CHECK_MSG(shaderID != g_InvalidID32, "Shader ID invalid");
	return ShaderDefHandle(*pRegMap, shaderID);
}

EffectDefHandle ShaderLibMap::GetEffect(uint effectID) const
{
	FX_CHECK_MSG(effectID != g_InvalidID32, "Effect ID invalid");
	return EffectDefHandle(*pRegMap, effectID);
}

uint ShaderLibMap::TryGetDefaultShaderVariant(uint nameID) const
{
	FX_CHECK_MSG(nameID != g_InvalidID32, "Name ID invalid");
	const auto& it = sharedVariantMap.shaders.find(nameID);

	if (it != sharedVariantMap.shaders.end())
		return it->second;
	else
		return g_InvalidID32;
}

uint ShaderLibMap::TryGetDefaultEffectVariant(uint nameID) const
{
	FX_CHECK_MSG(nameID != g_InvalidID32, "Name ID invalid");
	const auto& it = sharedVariantMap.effects.find(nameID);

	if (it != sharedVariantMap.effects.end())
		return it->second;
	else
		return g_InvalidID32;
}

uint ShaderLibMap::TryGetShaderID(uint nameID, uint vID) const 
{
	FX_CHECK_MSG(vID != g_InvalidID32 && nameID != g_InvalidID32, "Specified shader invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint cfgIndex = GetConfigIndex(vID);
	FX_CHECK_MSG(repoIndex >= 0 && cfgIndex >= 0
		&& repoIndex < variantShaderMaps.GetLength() 
		&& cfgIndex < variantShaderMaps[repoIndex].GetLength(),
	"Shader variant ID invalid");

	const NameIndexMap& nameMap = variantShaderMaps[repoIndex][cfgIndex].shaders;
	const auto& it = nameMap.find(nameID);

	if (it != nameMap.end())
		return it->second;
	else
		return g_InvalidID32;
}

uint ShaderLibMap::TryGetEffectID(uint nameID, uint vID) const 
{
	FX_CHECK_MSG(vID != g_InvalidID32 && nameID != g_InvalidID32, "Specified effect invalid");
	const uint repoIndex = GetRepoIndex(vID);
	const uint cfgIndex = GetConfigIndex(vID);
	FX_CHECK_MSG(repoIndex >= 0 && cfgIndex >= 0
		&& repoIndex < variantShaderMaps.GetLength()
		&& cfgIndex < variantShaderMaps[repoIndex].GetLength(),
		"Effect variant ID invalid");

	const NameIndexMap& nameMap = variantShaderMaps[repoIndex][cfgIndex].effects;
	const auto& it = nameMap.find(nameID);

	if (it != nameMap.end())
		return it->second;
	else
		return g_InvalidID32;
}

bool ShaderLibMap::GetIsDefined(uint nameID, uint vID) const 
{
	FX_CHECK_MSG(vID != g_InvalidID32, "Variant ID invalid");
	const uint repoID = GetRepoIndex(vID);
	const uint configID = GetConfigIndex(vID);
	
	return repoConfigTables[repoID].GetIsDefined(nameID, configID);
}

bool ShaderLibMap::GetIsDefined(string_view name, uint vID) const 
{
	if (uint id; GetStringMap().TryGetStringID(name, id))
		return GetIsDefined(id, vID);
	else
		return false;
}

void ShaderLibMap::GetDefines(uint vID, Vector<uint>& defines) const
{
	FX_CHECK_MSG(vID != g_InvalidID32, "Variant ID invalid");
	const uint repoID = GetRepoIndex(vID);
	const uint configID = GetConfigIndex(vID);
	
	return repoConfigTables[repoID].GetDefines(configID, defines);
}

uint ShaderLibMap::SetFlag(uint nameID, bool value, uint vID) const
{
	WV_CHECK_MSG(vID != g_InvalidID32, "Variant ID invalid");
	const uint repoID = GetRepoIndex(vID);
	const uint configID = GetConfigIndex(vID);

	return PackVariantID(repoID, repoConfigTables[repoID].SetFlag(nameID, value, configID));
}

uint ShaderLibMap::SetFlag(string_view name, bool value, uint vID) const 
{
	if (uint nameID; GetStringMap().TryGetStringID(name, nameID))
		return SetFlag(nameID, value, vID);
	else
		return g_InvalidID32;
}

uint ShaderLibMap::SetMode(uint nameID, uint vID) const
{
	const uint repoID = GetRepoIndex(vID);
	const uint configID = GetConfigIndex(vID);
	return PackVariantID(repoID, repoConfigTables[repoID].SetMode(nameID, configID));
}

uint ShaderLibMap::SetMode(string_view name, uint vID) const
{
	if (uint nameID; GetStringMap().TryGetStringID(name, nameID))
		return SetMode(nameID, vID);
	else
		return g_InvalidID32;
}

// Mask out lower 16 bits - configID
uint ShaderLibMap::ResetVariant(uint vID) const { return vID & ~g_VariantMask; }

uint ShaderLibMap::ResetMode(uint vID) const 
{ 
	const uint repoID = GetRepoIndex(vID);
	const uint configID = repoConfigTables[repoID].ResetMode(GetConfigIndex(vID));
	return PackVariantID(repoID, configID);
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

ShaderLibDef::Handle ShaderLibMap::GetDefinition() const
{
	return 
	{
		.pName = &name,
		.pPlatform = &platform,
		.pRepos = &variantRepos,
		.regHandle = pRegMap->GetDefinition(),
		.strMapHandle = pRegMap->GetStringMap().GetDefinition()
	};
}

