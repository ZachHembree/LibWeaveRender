#pragma once
#include <unordered_set>
#include <list>
#include "ReplicaUtils.hpp"
#include "boost/wave/cpp_exceptions.hpp"
#include "ShaderEntrypoint.hpp"

namespace Replica::Effects
{
	constexpr string_view VariantFlagsKeyword = "flags";
	constexpr uint VariantFlagLimit = 8;

	constexpr string_view VariantModesKeyword = "modes";
	constexpr uint VariantModeLimit = 256;

	/// <summary>
	/// Base class for exceptions thrown by Wave
	/// </summary>
	using WaveException = boost::wave::cpp_exception;

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
		void GetVariant(const int variantID, string& dst, Vector<ShaderEntrypoint>& entrypoints);

		/// <summary>
		/// Returns the total number of compile flag combos
		/// </summary>
		int GetFlagVariantCount() const;

		/// <summary>
		/// Returns the number of shader modes
		/// </summary>
		int GetShaderModeCount() const;

		/// <summary>
		/// Returns the number of unique shader variants. Only valid after variant 0 is
		/// generated.
		/// </summary>
		int GetVariantCount() const;

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
		const IDynamicArray<string_view>& GetVariantFlags() const;

		/// <summary>
		/// Returns the current list of variant modes
		/// </summary>
		const IDynamicArray<string_view>& GetVariantModes() const;

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

		UniqueVector<string> macroBuf;
		UniqueVector<string> sysIncludeBuf;
		UniqueVector<string> includeBuf;

		UniqueVector<string_view> variantModes;
		UniqueVector<string_view> variantFlags;
		Vector<ShaderEntrypoint>* pEntrypoints;

		std::unordered_set<string_view> variantDefineSet;
		std::list<string> variantDefineBuf;
	};
}