#pragma once
#include <unordered_map>
#include <concepts>
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "WeaveEffects/ConfigIDTable.hpp"

namespace Weave::Effects
{
	/// <summary>
	/// Provides a runtime interface to precompiled shader and effect variants.
	/// </summary>
	class ShaderLibMap
	{
	public:
		MAKE_NO_COPY(ShaderLibMap)

		ShaderLibMap();

		ShaderLibMap(const ShaderLibDef::Handle& def);

		ShaderLibMap(ShaderLibDef&& def);

		ShaderLibMap(const ShaderLibDef::Handle& def, StringIDBuilder& sharedStringIDs);

		ShaderLibMap(ShaderLibDef&& def, StringIDBuilder& sharedStringIDs);

		~ShaderLibMap();

		/// <summary>
		/// Returns the name of the shader library
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const IStringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns the number of shaders in a given variant
		/// </summary>
		uint GetShaderCount(uint vID) const;

		/// <summary>
		/// Returns the number of effects in a given variant
		/// </summary>
		uint GetEffectCount(uint vID) const;

		/// <summary>
		/// Returns a read only view of the map's definition
		/// </summary>
		ShaderLibDef::Handle GetDefinition() const;

		// Shader getters

		/// <summary>
		/// Returns the shader by shaderID
		/// </summary>
		ShaderDefHandle GetShader(uint shaderID) const;

		/// <summary>
		/// Returns the effect with the given effectID
		/// </summary>
		EffectDefHandle GetEffect(uint effectID) const;

		/// <summary>
		/// Returns the default variant ID for the shader with the given name, -1 on fail
		/// </summary>
		uint TryGetDefaultShaderVariant(uint nameID) const;

		/// <summary>
		/// Returns the default variant ID for the effect with the given name, -1 on fail
		/// </summary>
		uint TryGetDefaultEffectVariant(uint nameID) const;

		/// <summary>
		/// Returns the shaderID by name ID and variantID, -1 on fail
		/// </summary>
		uint TryGetShaderID(uint nameID, uint vID) const;

		/// <summary>
		/// Returns the effectID with the given name ID and variantID, -1 on fail
		/// </summary>
		uint TryGetEffectID(uint nameID, uint vID) const;

		// Flag/Mode getters

		/// <summary>
		/// Returns true if the given flag or mode is set for the given vID
		/// </summary>
		bool GetIsDefined(uint nameID, uint vID) const;

		/// <summary>
		/// Returns true if the given mode or flag name is set for the given variant
		/// </summary>
		bool GetIsDefined(string_view name, uint vID) const;

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(uint vID, Vector<uint>& defines) const;

		// Flag/Mode setters

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		uint SetFlag(uint nameID, bool value, uint vID) const;

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		uint SetFlag(string_view name, bool value, uint vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		uint SetMode(uint nameID, uint vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		uint SetMode(string_view name, uint vID) const;

		/// <summary>
		/// Returns default variant based on the given ID
		/// </summary>
		uint ResetVariant(uint vID) const;

		/// <summary>
		/// Resets variant mode to default
		/// </summary>
		uint ResetMode(uint vID) const;

	private:
		// (StringID) => ResourceID/Index
		using NameIndexMap = std::unordered_map<uint, uint>;

		struct VariantNameMap
		{
			/// <summary>
			/// Maps (ShaderName) => ShaderDef
			/// </summary>
			NameIndexMap shaders;

			/// <summary>
			/// Maps (EffectName) => EffectDef
			/// </summary>
			NameIndexMap effects;
		};

		string name;

		/// <summary>
		/// Describes the platform targeted during compilation
		/// </summary>
		PlatformDef platform;

		/// <summary>
		/// Pointer to unique shader data map
		/// </summary>
		std::unique_ptr<ShaderRegistryMap> pRegMap;

		/// <summary>
		/// NameID -> default vID map shared with all repos
		/// </summary>
		VariantNameMap sharedVariantMap;

		/// <summary>
		/// Stores shader and effect variant lookup tables
		/// </summary>
		UniqueArray<UniqueArray<VariantNameMap>> variantShaderMaps;

		/// <summary>
		/// Tables for encoding/decoding config IDs for a given repo
		/// </summary>
		UniqueArray<ConfigIDTable> repoConfigTables;

		/// <summary>
		/// Serializable variant repo data
		/// </summary>
		mutable UniqueArray<VariantRepoDef> variantRepos;

		void InitMaps();

	};
}