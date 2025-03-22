#pragma once
#include <unordered_map>
#include "ShaderDataHandles.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Faciltates retrieval of serialized and precompiled shaders and effects
	/// </summary>
	class ShaderLibMap
	{
	public:
		MAKE_MOVE_ONLY(ShaderLibMap)

		ShaderLibMap();

		ShaderLibMap(const ShaderLibDef& def);

		ShaderLibMap(ShaderLibDef&& def);

		~ShaderLibMap();

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns the shader by shaderID and variantID
		/// </summary>
		ShaderDefHandle GetShader(int shaderID, int vID = 0) const;

		/// <summary>
		/// Returns the effect with the given effectID and variantID
		/// </summary>
		EffectDefHandle GetEffect(int effectID, int vID = 0) const;

		/// <summary>
		/// Returns the shaderID by name ID and variantID, -1 on fail
		/// </summary>
		int TryGetShaderID(uint nameID, int vID = 0) const;

		/// <summary>
		/// Returns the effectID with the given name ID and variantID, -1 on fail
		/// </summary>
		int TryGetEffectID(uint nameID, int vID = 0) const;

		/// <summary>
		/// Finds the shader variant modeID corresponding to the given mode name
		/// </summary>
		int TryGetMode(uint nameID) const;

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <std::forward_iterator Iterator>
		int TryGetFlags(const Iterator begin, const Iterator end) const
		{
			uint flags = 0;

			for (Iterator it = begin; it != end; ++it)
			{
				const string_view& name = *it;
				uint stringID;

				if (GetStringMap().TryGetStringID(name, stringID))
				{ 
					const auto& result = flagNameMap.find(stringID);

					if (result != flagNameMap.end())
					{
						uint index = (uint)result->second;
						flags |= (1u << index);
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
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		int TryGetFlags(const std::initializer_list<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		int TryGetFlags(const IDynamicArray<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		int TryGetFlags(const std::vector<string_view>& defines) const { return TryGetFlags(defines.begin(), defines.end()); }

		/// <summary>
		/// Gets bit flag configuration using the given flag name(s)
		/// </summary>
		template <typename... T>
		int TryGetFlags(const T&... args) const { return TryGetFlags(std::initializer_list<string_view>{ args... }); }

		/// <summary>
		/// Calculates the bit flag configuration used from a given variant ID
		/// </summary>
		int GetFlagID(const int variantID) const;

		/// <summary>
		/// Calculates the modeID from a given variant ID
		/// </summary>
		int GetModeID(const int variantID) const;

		/// <summary>
		/// Calculates variantID from the given flag and mode configuration. Required for variant lookup.
		/// </summary>
		int GetVariantID(const int flagID, const int modeID) const;

		/// <summary>
		/// Returns true if the given flag or mode is set for the given vID
		/// </summary>
		bool GetIsDefined(uint nameID, const int vID) const;

		const IDynamicArray<string_view>& GetDefines(const int vID) const;

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
		size_t GetShaderCount(int vID = 0) const;

		/// <summary>
		/// Returns the number of effects in a given variant
		/// </summary>
		size_t GetEffectCount(int vID = 0) const;

	private:
		using NameIndexMap = std::unordered_map<uint, int>;

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
		std::unique_ptr<ShaderRegistryMap> pRegMap;

		/// <summary>
		/// Describes the platform targeted during compilation
		/// </summary>
		PlatformDef platform;

		/// <summary>
		/// Flag names used for static shader variant generation
		/// </summary>
		DynamicArray<uint> flagIDs;

		/// <summary>
		/// Mutually exclusive shader modes/features used for static shader variant generation
		/// </summary>
		DynamicArray<uint> modeIDs;

		/// <summary>
		/// Array of shaders and effects for each variant
		/// </summary>
		DynamicArray<VariantDef> variants;

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