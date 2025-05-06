#pragma once
#include <unordered_set>
#include <list>
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/StringSpan.hpp"
#include "ShaderEntrypoint.hpp"

namespace Weave::Effects
{
	constexpr string_view g_VariantFlagsKeyword = "flags";
	constexpr uint g_VariantFlagLimit = 8u;

	constexpr string_view g_VariantModesKeyword = "modes";
	constexpr uint g_VariantModeLimit = 256u;

	/// <summary>
	/// Evaluates preprocessor directives for shaders and generates variants
	/// </summary>
	class VariantPreprocessor
	{
	public:
		MAKE_MOVE_ONLY(VariantPreprocessor)

		VariantPreprocessor();

		/// <summary>
		/// Initializes preprocessor to the given source
		/// </summary>
		void SetSrc(string_view filePath, string_view src);

		/// <summary>
		/// Returns true if the preprocessor source has been set
		/// </summary>
		bool GetIsInitialized() const;

		/// <summary>
		/// Adds a preprocessor macro/define to the preprocessor symbol table
		/// </summary>
		void AddMacro(string_view macro);

		/// <summary>
		/// Adds an include path for system includes
		/// </summary>
		void AddSystemIncludePath(string_view path);

		/// <summary>
		/// Adds an include path for user includes
		/// </summary>
		void AddIncludePath(string_view path);
		
		/// <summary>
		/// Generates variant with flags corresponding to the given index and returns 
		/// reference to temporary buffer
		/// </summary>
		void GetVariant(const uint configID, string& dst, Vector<ShaderEntrypoint>& entrypoints);

		/// <summary>
		/// Returns the total number of compile flag combos
		/// </summary>
		uint GetFlagVariantCount() const;

		/// <summary>
		/// Returns the number of shader modes
		/// </summary>
		uint GetShaderModeCount() const;

		/// <summary>
		/// Returns the number of unique shader variants. Only valid after variant 0 is
		/// generated.
		/// </summary>
		uint GetVariantCount() const;

		/// <summary>
		/// Add external variant flag
		/// </summary>
		void AddVariantFlag(string_view flagName);

		/// <summary>
		/// Adds mutually exclusive mode variant
		/// </summary>
		void AddVariantMode(string_view modeName);

		/// <summary>
		/// Adds an entrypoint to the current variant
		/// </summary>
		void AddEntrypoint(string_view shaderName, ShadeStages stage);

		/// <summary>
		/// Returns the current list of variant flags
		/// </summary>
		const IDynamicArray<StringSpan>& GetVariantFlags() const;

		/// <summary>
		/// Returns the current list of variant modes
		/// </summary>
		const IDynamicArray<StringSpan>& GetVariantModes() const;

		/// <summary>
		/// Resets the preprocessor and clears internal state
		/// </summary>
		void Clear();

		/// <summary>
		/// Returns Boost.Wave version string
		/// </summary>
		static string GetWaveVerString();

	private:
		string_view src;
		string_view filePath;
		bool isInitialized;

		string textBuf;
		UniqueVector<StringSpan> macroStarts;
		UniqueVector<StringSpan> sysIncludeStarts;
		UniqueVector<StringSpan> includeStarts;
		UniqueVector<StringSpan> variantModes;
		UniqueVector<StringSpan> variantFlags;

		Vector<ShaderEntrypoint>* pEntrypoints;

		std::unordered_set<StringSpan> variantDefineSet;
	};
}