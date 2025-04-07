#pragma once
#include <unordered_map>
#include <concepts>
#include "ReplicaEffects/ShaderDataHandles.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Faciltates retrieval of preprocessed shaders and effects
	/// </summary>
	class VariantRepoMap
	{
	public:
		MAKE_NO_COPY(VariantRepoMap)

		VariantRepoMap();

		VariantRepoMap(const VariantRepoDef& repo, const ShaderRegistryMap& reg);

		VariantRepoMap(VariantRepoDef&& repo, const ShaderRegistryMap& reg) noexcept;

		~VariantRepoMap();
		
		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

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
		uint TryGetMode(uint nameID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template<typename Iterator>
		requires std::forward_iterator<Iterator>&&
			std::convertible_to<typename std::iterator_traits<Iterator>::value_type, std::string_view>
		uint TryGetFlags(const Iterator begin, const Iterator end) const
		{
			uint flags = 0;

			for (Iterator it = begin; it != end; ++it)
			{
				const string_view& name = *it;
				uint stringID;

				if (GetStringMap().TryGetStringID(name, stringID))
				{ 
					const auto& flagIt = flagNameMap.find(stringID);

					if (flagIt != flagNameMap.end())
					{
						uint flagIndex = (uint)flagIt->second;
						flags |= (1u << flagIndex);
					}
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
		requires std::forward_iterator<Iterator>&&
			std::is_integral_v<typename std::iterator_traits<Iterator>::value_type>
		uint TryGetFlags(const Iterator begin, const Iterator end) const
		{
			uint flags = 0;

			for (Iterator it = begin; it != end; ++it)
			{
				const auto& flagIt = flagNameMap.find(*it);

				if (flagIt != flagNameMap.end())
				{
					uint flagIndex = (uint)flagIt->second;
					flags |= (1u << flagIndex);
				}
				else
					return -1;
			}

			return flags;
		}

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		uint TryGetFlags(const std::initializer_list<string_view>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		uint TryGetFlags(const IDynamicArray<string_view>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T>
		requires std::convertible_to<T..., string_view>
		uint TryGetFlags(const T&... args) const { return TryGetFlags(std::initializer_list<string_view>{ args... }); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		uint TryGetFlags(const std::initializer_list<uint>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		uint TryGetFlags(const IDynamicArray<uint>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T>
		requires std::convertible_to<T..., uint>
		uint TryGetFlags(const T&... args) const { return TryGetFlags(std::initializer_list<uint>{ args... }); }

		/// <summary>
		/// Calculates the bit flag configuration used from a given variant ID
		/// </summary>
		uint GetFlagID(uint variantID) const;

		/// <summary>
		/// Calculates the modeID from a given variant ID
		/// </summary>
		uint GetModeID(uint variantID) const;

		/// <summary>
		/// Calculates variantID from the given flag and mode configuration. Required for variant lookup.
		/// </summary>
		uint GetVariantID(uint flagID, uint modeID) const;

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
		void GetDefines(const uint vID, Vector<uint>& defines) const;

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(const uint vID, Vector<string_view>& defines) const;

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
		/// Resets variant mode to default
		/// </summary>
		uint ResetMode(uint vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		uint SetMode(string_view name, uint vID) const;

		/// <summary>
		/// Returns an immutable reference to library variant definitions
		/// </summary>
		const IDynamicArray<VariantDef>& GetVariants() const;

		/// <summary>
		/// Returns the total number of flag combinations used for variant generation
		/// </summary>
		uint GetFlagVariantCount() const;

		/// <summary>
		/// Returns the total number of modes used for variant generation
		/// </summary>
		uint GetModeCount() const;

		/// <summary>
		/// Returns the total number of variants generated, including all flag and mode combinations
		/// </summary>
		uint GetVariantCount() const;

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
			NameIndexMap nameShaderMap;

			/// <summary>
			/// Maps (EffectName) => EffectDef
			/// </summary>
			NameIndexMap effectNameMap;

			VariantNameMap()
			{ }
		};

		/// <summary>
		/// Pointer to unique shader data map
		/// </summary>
		const ShaderRegistryMap* pRegMap;

		VariantRepoDef repoData;

		/// <summary>
		/// Flag names used for static shader variant generation
		/// </summary>
		NameIndexMap flagNameMap;

		/// <summary>
		/// Mutually exclusive shader modes/features used for static shader variant generation
		/// </summary>
		NameIndexMap modeNameMap;

		/// <summary>
		/// Stores shader and effect variant lookup tables
		/// </summary>
		UniqueArray<VariantNameMap> variantMaps;

		void InitMap();

		const VariantDef& GetVariant(const uint vID) const;
	};
}