#include "pch.hpp"
#include "WeaveEffects/ConfigIDTable.hpp"
#include "WeaveUtils/Logger.hpp"

using namespace Weave;
using namespace Weave::Effects;

static uint GetConfigFlags(uint configID, uint fvCount) { return configID % fvCount; }

static uint GetConfigMode(uint configID, uint fvCount) { return configID / fvCount; }

static uint PackConfigID(uint flagID, uint modeID, uint fvCount) { return flagID + (modeID * fvCount); }

DEF_MOVE_ONLY(ConfigIDTable);

ConfigIDTable::ConfigIDTable() = default;

ConfigIDTable::ConfigIDTable(ConfigIDTableDef& def, const IStringIDMap& stringIDs) :
	pDef(&def)
{
	InitTable(stringIDs);
}

ConfigIDTable::~ConfigIDTable() = default;

void ConfigIDTable::InitTable(const IStringIDMap& stringIDs)
{
	// Shader flags
	for (uint i = 0; i < pDef->flagIDs.GetLength(); i++)
	{
		const uint flagID = 1u << i;
		// Translate flag IDs if aliased
		pDef->flagIDs[i] = stringIDs.GetAliasedID(pDef->flagIDs[i]);
		nameFlagMap[pDef->flagIDs[i]] = flagID;
	}

	// Shader modes
	for (uint i = 0; i < pDef->modeIDs.GetLength(); i++)
	{
		// Translate mode IDs if aliased
		pDef->modeIDs[i] = stringIDs.GetAliasedID(pDef->modeIDs[i]);
		nameModeMap[pDef->modeIDs[i]] = i;
	}
}

uint ConfigIDTable::SetFlag(uint nameID, bool value, uint configID) const
{
	const uint flagID = TryGetFlagIndex(nameID);
	const uint fvCount = GetFlagCombinations();
	const uint mode = GetConfigMode(configID, fvCount);
	uint newFlags = GetConfigFlags(configID, fvCount);

	if (value)
		newFlags |= flagID;
	else
		newFlags &= ~flagID;

	return PackConfigID(newFlags, mode, fvCount);
}

uint ConfigIDTable::SetMode(uint nameID, uint configID) const
{
	const uint modeID = TryGetModeIndex(nameID);
	const uint fvCount = GetFlagCombinations();
	const uint flags = GetConfigFlags(configID, fvCount);
	return PackConfigID(flags, modeID, fvCount);
}

bool ConfigIDTable::GetIsDefined(uint nameID, uint configID) const
{
	FX_CHECK_MSG(nameID != g_InvalidID32, "Name ID invalid");

	// Check flags
	const auto& flagIt = nameFlagMap.find(nameID);
	
	if (flagIt != nameFlagMap.end())
	{
		const uint flags = GetConfigFlags(configID, GetFlagCombinations());
		const uint flag = flagIt->second;
		return (flags & flag) == flag;
	}
	// Check modes
	else
	{
		const uint mode = GetModeIndex(configID);
		const auto& modeIt = nameModeMap.find(nameID);

		if (modeIt != nameModeMap.end())
			return mode == modeIt->second;
		else
			return false;
	}
}

void ConfigIDTable::GetDefines(uint configID, Vector<uint>& defines) const
{
	const uint fvCount = GetFlagCombinations();
	const uint modeID = GetModeIndex(configID);
	uint flagID = GetFlags(configID);

	WV_CHECK_MSG(flagID < fvCount && modeID < GetModeCount(), "ConfigID invalid");

	if (modeID != 0)
		defines.EmplaceBack(pDef->modeIDs[modeID]);

	uint flagIndex = 0u;

	while (flagID > 0u)
	{
		if ((flagID & 1u) == 1u)
			defines.EmplaceBack(pDef->flagIDs[flagIndex]);

		flagIndex++;
		flagID >>= 1u;
	}
}

uint ConfigIDTable::SetDefines(const IDynamicArray<uint>& defines) const
{
	uint modeID = 0;
	uint flagID = 0;

	for (uint stringID : defines)
	{
		if (const uint newMode = TryGetModeIndex(stringID); newMode != -1)
		{
			if (modeID != 0)
				WV_LOG_WARN_FMT("Mutually exclusive shader modes were specified in the same group of defines. "
				"ModeID: {} is invalid.", newMode);

			modeID = newMode;
		}
		else if (const uint newFlag = TryGetFlagIndex(stringID); newFlag != -1)
			flagID |= newFlag;
		else
			WV_LOG_WARN_FMT("Shader define ({}) was not recognized and is being ignored.", stringID);
	}

	return PackConfigID(flagID, modeID, GetFlagCombinations());
}

const ConfigIDTableDef& ConfigIDTable::GetDefinition() const { return *pDef; }

uint ConfigIDTable::ResetMode(uint configID) const
{
	return PackConfigID(GetFlags(configID), 0, GetFlagCombinations());
}

uint ConfigIDTable::ResetFlags(uint configID) const
{
	return PackConfigID(0, GetModeIndex(configID), GetFlagCombinations());
}

uint ConfigIDTable::TryGetModeIndex(uint nameID) const
{
	WV_CHECK_MSG(nameID != g_InvalidID32, "Specified mode name is invalid");
	const auto& it = nameModeMap.find(nameID);

	if (it != nameModeMap.end())
		return it->second;
	else
		return g_InvalidID32;
}

uint ConfigIDTable::TryGetFlagIndex(uint nameID) const
{
	WV_CHECK_MSG(nameID != g_InvalidID32, "Specified flag name is invalid");
	const auto& it = nameFlagMap.find(nameID);

	if (it != nameFlagMap.end())
		return it->second;
	else
		return g_InvalidID32;
}

uint ConfigIDTable::GetFlags(uint configID) const { return GetConfigFlags(configID, GetFlagCombinations()); }

uint ConfigIDTable::GetModeIndex(uint configID) const { return GetConfigMode(configID, GetFlagCombinations()); }

uint ConfigIDTable::GetFlagCombinations() const { return 1u << (uint)pDef->flagIDs.GetLength(); }

uint ConfigIDTable::GetModeCount() const { return (uint)pDef->modeIDs.GetLength(); }