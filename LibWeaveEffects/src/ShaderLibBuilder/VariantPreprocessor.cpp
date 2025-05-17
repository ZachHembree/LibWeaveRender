#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/VariantPreprocessor.hpp"

namespace Weave::Effects
{ 
	static StringSpan AddStringSpan(string_view substr, Vector<StringSpan>& spanBuf, string& textBuf)
	{
		const uint bufStart = (uint)textBuf.length();
		uint subLen = (uint)substr.length();
		textBuf.append(substr);

		if (substr.back() != '\0')
		{
			textBuf.push_back('\0');
			subLen++;
		}

		return spanBuf.EmplaceBack(textBuf, bufStart, subLen);
	}

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

	void VariantPreprocessor::AddMacro(std::string_view macro) { AddStringSpan(macro, macroStarts, textBuf); }

	void VariantPreprocessor::AddSystemIncludePath(std::string_view path) { AddStringSpan(path, sysIncludeStarts, textBuf); }

	void VariantPreprocessor::AddIncludePath(std::string_view path) { AddStringSpan(path, includeStarts, textBuf); }

	void VariantPreprocessor::Clear()
	{
		src = std::string_view();
		filePath = std::string_view();
		isInitialized = false;

		variantDefineSet.clear();
		variantModes.Clear();
		variantFlags.Clear();

		textBuf.clear();
		macroStarts.Clear();
		sysIncludeStarts.Clear();
		includeStarts.Clear();
	
		pEntrypoints = nullptr;

		AddVariantMode("__DEFAULT_SHADER_MODE__");
	}

	std::string VariantPreprocessor::GetWaveVerString() { return WaveContext::get_version_string(); }

	void VariantPreprocessor::GetVariant(const uint configID, string& dst, Vector<ShaderEntrypoint>& entrypoints)
	{
		FX_ASSERT_MSG(configID != -1 && configID < std::max<uint>(1u, GetVariantCount()), "Invalid variant ID");
		pEntrypoints = &entrypoints;

		// Initialize context to source
		WaveStringView str(src.data(), src.length());
		WaveContext ctx = WaveContext(str.begin(), str.end(), filePath.data(), *this);
		ctx.set_language(VarProcLangFlags);

		// Configure include paths
		for (const StringSpan& sysInclude : sysIncludeStarts)
			ctx.add_sysinclude_path(sysInclude.GetFirst());

		for (const StringSpan& include : includeStarts)
			ctx.add_include_path(include.GetFirst());

		// Configure external macros
		for (const StringSpan& macro : macroStarts)
			ctx.add_macro_definition(macro);

		// Convert ID into bit flags and set defines
		const uint flagID = configID % GetFlagVariantCount();

		for (int i = 0; i < variantFlags.GetLength(); i++)
		{
			const uint id = 1u << i;

			if ((id & flagID) > 0)
				ctx.add_macro_definition(variantFlags[i]);
		}

		// Set mode define
		const uint enumID = configID / GetFlagVariantCount();

		if (enumID > 0)
			ctx.add_macro_definition(variantModes[enumID]);

		// Process directives
		WaveTokenIterator it = ctx.begin();
		WaveTokenIterator last = ctx.end();

		while (true)
		{
			try
			{
				if (it == last)
					break;

				WaveLexToken& token = *it;
				const WaveString& value = token.get_value();
				std::copy(value.begin(), value.end(), std::back_inserter(dst));
			}
			catch (const WaveException& err)
			{
				if (!err.is_recoverable())
					FXSYNTAX_THROW("{}\nFile: {}\nLine: {}", err.description(), err.file_name(), err.line_no());
				else
					WV_LOG_WARN() << err.description() << "\nFile: " << err.file_name() << "\nLine: " << err.line_no();
			}

			++it;
		}

		isInitialized = true;
		pEntrypoints = nullptr;
	}

	uint VariantPreprocessor::GetFlagVariantCount() const { return 1u << (variantFlags.GetLength()); }

	uint VariantPreprocessor::GetShaderModeCount() const { return (uint)variantModes.GetLength(); }

	uint VariantPreprocessor::GetVariantCount() const { return GetFlagVariantCount() * std::max(1u, GetShaderModeCount()); }

	void VariantPreprocessor::AddVariantFlag(string_view flag) 
	{
		if (!isInitialized)
		{
			const StringSpan& span = AddStringSpan(flag, variantFlags, textBuf);

			if (!variantDefineSet.contains(span))
			{ 
				FXSYNTAX_CHECK_MSG(variantFlags.GetLength() < g_VariantFlagLimit, "Flag pragma limit exceeded");
				variantDefineSet.emplace(span);
			}
			else
				FXSYNTAX_THROW("Attempted to redefine pragma flag or mode");
		}
	}

	void VariantPreprocessor::AddVariantMode(string_view modeName)
	{
		if (!isInitialized)
		{
			const StringSpan& span = AddStringSpan(modeName, variantModes, textBuf);

			if (!variantDefineSet.contains(span))
			{ 
				FXSYNTAX_CHECK_MSG(variantModes.GetLength() < g_VariantModeLimit, "Mode pragma limit exceeded");
				variantDefineSet.emplace(span);
			}
			else
				FXSYNTAX_THROW("Attempted to redefine pragma flag or mode");
		}
	}

	void VariantPreprocessor::AddEntrypoint(string_view shaderName, ShadeStages stage)
	{
		pEntrypoints->EmplaceBack(string(shaderName), stage);
	}

	const IDynamicArray<StringSpan>& VariantPreprocessor::GetVariantFlags() const { return variantFlags; }

	const IDynamicArray<StringSpan>& VariantPreprocessor::GetVariantModes() const { return variantModes; }

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

		if (optName == g_VariantFlagsKeyword)
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
		else if (optName == g_VariantModesKeyword)
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