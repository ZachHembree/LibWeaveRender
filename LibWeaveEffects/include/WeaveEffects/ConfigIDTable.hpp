#pragma once
#include <unordered_map>
#include <concepts>
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "WeaveUtils/StringIDMap.hpp"

namespace Weave::Effects
{
	class ConfigIDTable
	{
	public:
		DECL_MOVE_ONLY(ConfigIDTable);

		ConfigIDTable();

		ConfigIDTable(ConfigIDTableDef& def, const IStringIDMap& stringIDs);

		~ConfigIDTable();

		// Main utils

		/// <summary>
		/// Sets mode by name for the given config and returns the new configID
		/// </summary>
		uint SetMode(uint nameID, uint configID) const;

		/// <summary>
		/// Sets flag by name for the given config and returns the new configID
		/// </summary>
		uint SetFlag(uint nameID, bool value, uint configID) const;

		/// <summary>
		/// Returns true if the given ID sets the flag/mode corresponding to the given name's
		/// string ID
		/// </summary>
		bool GetIsDefined(uint nameID, uint configID) const;

		/// <summary>
		/// Resets mode for the given configuration to defaults while preserving flags
		/// </summary>
		uint ResetMode(uint configID) const;

		/// <summary>
		/// Resets flags for the given configuration to defaults while preserving the mode
		/// </summary>
		uint ResetFlags(uint configID) const;

		// Collection utils

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(uint configID, Vector<uint>& defineNameIDs) const;

		/// <summary>
		/// Builds a configID from the given set of define name IDs
		/// </summary>
		uint SetDefines(const IDynamicArray<uint>& defineNameIDs) const;

		const ConfigIDTableDef& GetDefinition() const;

	private:
		// Flags and modes
		ConfigIDTableDef* pDef;

		// Define nameID -> define ID lookup tables
		std::unordered_map<uint, uint> nameFlagMap;
		std::unordered_map<uint, uint> nameModeMap;

		void InitTable(const IStringIDMap& stringIDs);

		/// <summary>
		/// Returns internal mode index by name
		/// </summary>
		uint TryGetModeIndex(uint nameID) const;

		/// <summary>
		/// Returns internal flag index by name
		/// </summary>
		uint TryGetFlagIndex(uint nameID) const;

		/// <summary>
		/// Calculates the combined bit flag configuration used from a given config ID
		/// </summary>
		uint GetFlags(uint configID) const;

		/// <summary>
		/// Calculates the modeID from a given variant ID
		/// </summary>
		uint GetModeIndex(uint configID) const;

		/// <summary>
		/// Returns the total number of flag combinations used for variant generation
		/// </summary>
		uint GetFlagCombinations() const;

		/// <summary>
		/// Returns the total number of modes used for variant generation
		/// </summary>
		uint GetModeCount() const;
	};
}