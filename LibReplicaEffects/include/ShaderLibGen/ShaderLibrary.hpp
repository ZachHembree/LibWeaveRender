#pragma once
#include <unordered_map>
#include "ShaderParser/ShaderData.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Faciltates retrieval of serialized and precompiled shaders and effects
	/// </summary>
	class ShaderLibrary
	{
	public:
		MAKE_MOVE_ONLY(ShaderLibrary)

		ShaderLibrary();

		ShaderLibrary(const ShaderLibDef* pDef);

		/// <summary>
		/// Returns the shader by name and variantID
		/// </summary>
		const ShaderDef* TryGetShader(string_view name, int vID = 0) const;

		/// <summary>
		/// Returns the effect with the given name and variantID
		/// </summary>
		const EffectDef* TryGetEffect(string_view name, int vID = 0) const;

		/// <summary>
		/// Finds the shader variant modeID corresponding to the given mode name
		/// </summary>
		int TryGetMode(const string_view name) const;

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
				const auto& result = flagNameMap.find(name);

				if (result != flagNameMap.end())
				{
					uint index = (uint)result->second;
					flags |= (1u << index);
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

	private:
		using NameIndexMap = std::unordered_map<string_view, int>;

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
		/// Pointer to serializable library data
		/// </summary>
		const ShaderLibDef* const pDef;

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
	};
}