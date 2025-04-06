#pragma once
#include <unordered_map>
#include <concepts>
#include "ReplicaEffects/ShaderDataHandles.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Faciltates retrieval of preprocessed shaders and effects
	/// </summary>
	class ShaderRepoMap
	{
	public:
		MAKE_NO_COPY(ShaderRepoMap)

		ShaderRepoMap();

		ShaderRepoMap(const VariantRepoDef& repo, const ShaderRegistryMap& reg);

		ShaderRepoMap(VariantRepoDef&& repo, const ShaderRegistryMap& reg) noexcept;

		~ShaderRepoMap();
		
		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns the shaderID by name ID and variantID, -1 on fail
		/// </summary>
		uint TryGetShaderID(uint nameID, int vID) const;

		/// <summary>
		/// Returns the effectID with the given name ID and variantID, -1 on fail
		/// </summary>
		uint TryGetEffectID(uint nameID, int vID) const;

		/// <summary>
		/// Finds the shader variant modeID corresponding to the given mode name
		/// </summary>
		int TryGetMode(uint nameID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template<typename Iterator>
		requires std::forward_iterator<Iterator>&&
			std::convertible_to<typename std::iterator_traits<Iterator>::value_type, std::string_view>
		int TryGetFlags(const Iterator begin, const Iterator end) const
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

			return (int)flags;
		}

		/// <summary>
		/// Gets bit flag configuration from the stringIDs corresponding to flag names
		/// </summary>
		template<typename Iterator>	
		requires std::forward_iterator<Iterator>&&
			std::is_integral_v<typename std::iterator_traits<Iterator>::value_type>
		int TryGetFlags(const Iterator begin, const Iterator end) const
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

			return (int)flags;
		}

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		int TryGetFlags(const std::initializer_list<string_view>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		int TryGetFlags(const IDynamicArray<string_view>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T>
		requires std::convertible_to<T..., string_view>
		int TryGetFlags(const T&... args) const { return TryGetFlags(std::initializer_list<string_view>{ args... }); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		int TryGetFlags(const std::initializer_list<uint>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name IDs
		/// </summary>
		int TryGetFlags(const IDynamicArray<uint>& defines) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T>
		requires std::convertible_to<T..., uint>
		int TryGetFlags(const T&... args) const { return TryGetFlags(std::initializer_list<uint>{ args... }); }

		/// <summary>
		/// Calculates the bit flag configuration used from a given variant ID
		/// </summary>
		uint GetFlagID(const int variantID) const;

		/// <summary>
		/// Calculates the modeID from a given variant ID
		/// </summary>
		uint GetModeID(const int variantID) const;

		/// <summary>
		/// Calculates variantID from the given flag and mode configuration. Required for variant lookup.
		/// </summary>
		int GetVariantID(const uint flagID, const uint modeID) const;

		/// <summary>
		/// Returns true if the given flag or mode is set for the given vID
		/// </summary>
		bool GetIsDefined(uint nameID, const int vID) const;

		/// <summary>
		/// Returns true if the given mode or flag name is set for the given variant
		/// </summary>
		bool GetIsDefined(string_view name, const int vID) const;

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(const int vID, Vector<uint>& defines) const;

		/// <summary>
		/// Returns all mode and flag names set for the given variant
		/// </summary>
		void GetDefines(const int vID, Vector<string_view>& defines) const;

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		int SetFlag(uint nameID, bool value, int vID) const;

		/// <summary>
		/// Returns variantID with the given flag set to the given value.
		/// </summary>
		int SetFlag(string_view name, bool value, int vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		int SetMode(uint nameID, int vID) const;

		/// <summary>
		/// Resets variant mode to default
		/// </summary>
		int ResetMode(int vID) const;

		/// <summary>
		/// Returns variantID with the given mode
		/// </summary>
		int SetMode(string_view name, int vID) const;

		/// <summary>
		/// Returns an immutable reference to library variant definitions
		/// </summary>
		const IDynamicArray<VariantDef>& GetVariants() const;

		/// <summary>
		/// Returns the total number of flag combinations used for variant generation
		/// </summary>
		size_t GetFlagVariantCount() const;

		/// <summary>
		/// Returns the total number of modes used for variant generation
		/// </summary>
		size_t GetModeCount() const;

		/// <summary>
		/// Returns the total number of variants generated, including all flag and mode combinations
		/// </summary>
		size_t GetVariantCount() const;

		/// <summary>
		/// Returns the number of shaders in a given variant
		/// </summary>
		size_t GetShaderCount(int vID) const;

		/// <summary>
		/// Returns the number of effects in a given variant
		/// </summary>
		size_t GetEffectCount(int vID) const;

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

		const VariantDef& GetVariant(const int vID) const;
	};
}