#include "ShaderLibGen/WaveConfig.hpp"
#include "ShaderLibGen/VariantPreprocessor.hpp"
#include "ParseExcept.hpp"

namespace Replica::Effects
{ 
	VariantPreprocessor::VariantPreprocessor() : 
		isInitialized(false), 
		pEntrypoints(nullptr)
	{ }

	void VariantPreprocessor::SetSrc(string_view filePath, string_view src)
	{
		Clear();
		this->src = src;
		this->filePath = filePath;
	}

	bool VariantPreprocessor::GetIsInitialized() const { return isInitialized; }

	void VariantPreprocessor::AddMacro(std::string_view macro) { macroBuf.emplace_back(macro); }

	void VariantPreprocessor::AddSystemIncludePath(std::string_view path) { sysIncludeBuf.emplace_back(path); }

	void VariantPreprocessor::AddIncludePath(std::string_view path) { includeBuf.emplace_back(path); }

	void VariantPreprocessor::Clear()
	{
		src = std::string_view();
		filePath = std::string_view();
		isInitialized = false;

		macroBuf.clear();
		sysIncludeBuf.clear();
		includeBuf.clear();

		variantDefineSet.clear();
		variantDefineBuf.clear();

		variantModes.clear();
		variantFlags.clear();
		pEntrypoints = nullptr;

		AddVariantMode("__DEFAULT_SHADER_MODE__");
	}

	std::string VariantPreprocessor::GetWaveVerString() { return WaveContext::get_version_string(); }

	void VariantPreprocessor::GetVariant(const int variantID, string& dst, Vector<ShaderEntrypoint>& entrypoints)
	{
		PARSE_ASSERT_MSG(variantID >= 0 && variantID < std::max<size_t>(1, GetVariantCount()), "Invalid variant ID");
		pEntrypoints = &entrypoints;

		// Initialize context to source
		WaveStringView str(src.data(), src.length());
		WaveContext ctx = WaveContext(str.begin(), str.end(), filePath.data(), *this);
		ctx.set_language(VarProcLangFlags);

		// Configure include paths
		for (const string& sysInclude : sysIncludeBuf)
			ctx.add_sysinclude_path(sysInclude.data());

		for (const string& include : includeBuf)
			ctx.add_include_path(include.data());

		// Configure external macros
		for (const string& macro : macroBuf)
			ctx.add_macro_definition(macro);

		// Convert ID into bit flags and set defines
		const int flagID = variantID % GetFlagVariantCount();

		for (int i = 0; i < variantFlags.GetLength(); i++)
		{
			const int id = 1 << i;

			if ((id & flagID) > 0)
				ctx.add_macro_definition(variantFlags[i].data());
		}

		// Set mode define
		const int enumID = variantID / GetFlagVariantCount();

		if (enumID > 0)
			ctx.add_macro_definition(variantModes[enumID].data());

		// Process directives
		for (const WaveLexToken& token : ctx)
		{
			const WaveString& value = token.get_value();
			std::copy(value.begin(), value.end(), std::back_inserter(dst));
		}

		isInitialized = true;
		pEntrypoints = nullptr;
	}

	int VariantPreprocessor::GetFlagVariantCount() const { return 1 << (variantFlags.GetLength()); }

	int VariantPreprocessor::GetShaderModeCount() const { return (int)variantModes.GetLength(); }

	int VariantPreprocessor::GetVariantCount() const { return GetFlagVariantCount() * std::max(1, GetShaderModeCount()); }

	void VariantPreprocessor::AddVariantFlag(string_view flag) 
	{
		if (!isInitialized)
		{
			if (!variantDefineSet.contains(flag))
			{ 
				PARSE_ASSERT_MSG(variantFlags.GetLength() < VariantFlagLimit, "Flag pragma limit exceeded")

				string& flagCpy = variantDefineBuf.emplace_back(flag);
				variantDefineSet.emplace(variantFlags.emplace_back(flagCpy));
			}
			else
				PARSE_ERR("Attempted to redefine pragma flag or mode")
		}
	}

	void VariantPreprocessor::AddVariantMode(string_view modeName)
	{
		if (!isInitialized)
		{
			if (!variantDefineSet.contains(modeName))
			{ 
				PARSE_ASSERT_MSG(variantModes.GetLength() < VariantModeLimit, "Mode pragma limit exceeded")
			
				string& flagCpy = variantDefineBuf.emplace_back(modeName);
				variantDefineSet.emplace(variantModes.emplace_back(flagCpy));
			}
			else
				PARSE_ERR("Attempted to redefine pragma flag or mode")
		}
	}

	void VariantPreprocessor::AddEntrypoint(string_view shaderName, ShadeStages stage)
	{
		pEntrypoints->emplace_back(string(shaderName), stage);
	}

	const IDynamicArray<string_view>& VariantPreprocessor::GetVariantFlags() const { return variantFlags; }

	const IDynamicArray<string_view>& VariantPreprocessor::GetVariantModes() const { return variantModes; }

	WaveContextPolicy::WaveContextPolicy() : pMain(nullptr)
	{ }

	WaveContextPolicy::WaveContextPolicy(VariantPreprocessor & procMain) : pMain(&procMain)
	{ }

	template<>
	bool WaveContextPolicy::interpret_pragma(
		WaveContext const& ctx,
		WaveLexTokenSeq& pending,
		WaveLexToken const& option,
		WaveLexTokenSeq const& values,
		WaveLexToken const& pragma_token)
	{
		if (pMain == nullptr)
			return false;
			
		const WaveString& optString = option.get_value();
		const string_view optName(optString.begin(), optString.end());
		ShadeStages stage;

		if (optName == VariantFlagsKeyword)
		{
			for (const WaveLexToken& lexToken : values)
			{
				if (lexToken == boost::wave::T_IDENTIFIER)
				{
					const WaveString& value = lexToken.get_value();
					pMain->AddVariantFlag(string_view(value.begin(), value.end()));
				}
			}

			return true;
		}
		else if (optName == VariantModesKeyword)
		{
			for (const WaveLexToken& lexToken : values)
			{
				if (lexToken == boost::wave::T_IDENTIFIER)
				{
					const WaveString& value = lexToken.get_value();
					pMain->AddVariantMode(string_view(value.begin(), value.end()));
				}
			}

			return true;
		}
		else if (TryGetShadeStage(optName, stage))
		{
			for (const WaveLexToken& lexToken : values)
			{
				if (lexToken == boost::wave::T_IDENTIFIER)
				{
					const WaveString& value = lexToken.get_value();
					pMain->AddEntrypoint(string_view(value.begin(), value.end()), stage);
				}
			}
			
			return true;
		}
		else
			return false;
	}
}