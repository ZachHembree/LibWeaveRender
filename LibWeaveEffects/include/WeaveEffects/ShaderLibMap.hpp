#pragma once
#include <unordered_map>
#include <concepts>
#include "WeaveEffects/ShaderDataHandles.hpp"

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

		~ShaderLibMap();

		/// <summary>
		/// Returns the name of the shader library
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns an array of all variant repo definitions
		/// </summary>
		const IDynamicArray<VariantRepoDef>& GetRepos() const;

		/// <summary>
		/// Returns all flag IDs used by the variant repo associated with the given ID
		/// </summary>
		const IDynamicArray<uint>& GetFlags(uint vID) const;

		/// <summary>
		/// Returns all mode IDs used by the variant repo associated with the given ID
		/// </summary>
		const IDynamicArray<uint>& GetModes(uint vID) const;

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

		/// <summary>
		/// Finds the shader variant modeID corresponding to the given mode name
		/// </summary>
		uint TryGetModeID(uint nameID, uint vID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template<typename Iterator>
			requires std::forward_iterator<Iterator> &&
			std::convertible_to<typename std::iterator_traits<Iterator>::value_type, std::string_view>
		uint TryGetFlags(const Iterator begin, const Iterator end, uint vID) const
		{
			uint flags = 0;

			for (Iterator it = begin; it != end; ++it)
			{
				const string_view& name = *it;
				uint stringID;

				if (GetStringMap().TryGetStringID(name, stringID))
				{
					const uint flag = TryGetFlag(stringID, vID);

					if (flag != -1)
						flags |= flag;
					else
						return -1;
				}
				else
					return -1;
			}

			return flags;
		}

		/// <summary>
		/// Gets bit flag configuration from the stringIDs corresponding to flag names
		/// </summary>
		template<typename Iterator>
			requires std::forward_iterator<Iterator> &&
			std::is_integral_v<typename std::iterator_traits<Iterator>::value_type>
		uint TryGetFlags(const Iterator begin, const Iterator end, uint vID) const
		{
			uint flags = 0;

			for (Iterator it = begin; it != end; ++it)
			{
				const uint flag = TryGetFlag(*it, vID);

				if (flag != -1)
					flags |= flag;
				else
					return -1;
			}

			return flags;
		}

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		uint TryGetFlags(const std::initializer_list<string_view>& defines, uint vID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		uint TryGetFlags(const IDynamicArray<string_view>& defines, uint vID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T> requires std::convertible_to<T..., string_view>
		uint TryGetFlags(const T&... args, uint vID) const { return TryGetFlags(std::initializer_list<string_view>{ args... }, vID); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		uint TryGetFlags(const std::initializer_list<uint>& defines, uint vID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		uint TryGetFlags(const IDynamicArray<uint>& defines, uint vID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T> requires std::convertible_to<T..., uint>
		uint TryGetFlags(const T&... args, uint vID) const { return TryGetFlags(std::initializer_list<uint>{ args... }, vID); }

		/// <summary>
		/// Gets the flagID associated with the given name and variant. -1 on fail.
		/// </summary>
		uint TryGetFlag(uint nameID, uint vID) const;

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

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(uint vID, Vector<string_view>& defines) const;

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		uint SetFlag(uint nameID, bool value, uint vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		uint SetMode(uint modeID, uint vID) const;

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		uint SetFlag(string_view name, bool value, uint vID) const;

		/// <summary>
		/// Returns default variant based on the given ID
		/// </summary>
		uint ResetVariant(uint vID) const;

		/// <summary>
		/// Resets variant mode to default
		/// </summary>
		uint ResetMode(uint vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		uint SetMode(string_view name, uint vID) const;

		/// <summary>
		/// Returns the number of shaders in a given variant
		/// </summary>
		uint GetShaderCount(uint vID) const;

		/// <summary>
		/// Returns the number of effects in a given variant
		/// </summary>
		uint GetEffectCount(uint vID) const;

	private:
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
		/// Flag names used for static shader variant generation
		/// </summary>
		UniqueArray<NameIndexMap> variantFlagMaps;

		/// <summary>
		/// Mutually exclusive shader modes/features used for static shader variant generation
		/// </summary>
		UniqueArray<NameIndexMap> variantModeMaps;

		/// <summary>
		/// Serializable variant repo data
		/// </summary>
		UniqueArray<VariantRepoDef> variantRepos;

		void InitMaps();

		/// <summary>
		/// Calculates the combined bit flag configuration used from a given variant ID
		/// </summary>
		uint GetFlags(uint repoIndex, uint configIndex) const;

		/// <summary>
		/// Calculates the modeID from a given variant ID
		/// </summary>
		uint GetModeIndex(uint repoIndex, uint configIndex) const;

		/// <summary>
		/// Calculates variantID from the given flag and mode configuration. Required for variant lookup.
		/// </summary>
		uint GetVariantID(uint repoIndex, uint flagID, uint modeID) const;

		/// <summary>
		/// Returns the total number of flag combinations used for variant generation
		/// </summary>
		uint GetFlagVariantCount(uint repoIndex) const;

		/// <summary>
		/// Returns the total number of modes used for variant generation
		/// </summary>
		uint GetModeCount(uint vID) const;
	};
}