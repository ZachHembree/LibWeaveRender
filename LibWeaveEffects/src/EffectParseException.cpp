#include "pch.hpp"
#include "WeaveEffects/EffectParseException.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"

using namespace Weave;
using namespace Weave::Effects;

string_view EffectParseException::GetType() const noexcept
{
	return "FX Parse Error";
}

EffectParseException::EffectParseException(string&& msg) :
	WeaveException(std::move(msg))
{ }

EffectParseException::EffectParseException(const std::source_location& loc, string&& msg)
{
	if (msg.length() > 0)
	{
		this->msg = std::format(
			"{}\n"
			"[DEBUG INTERNAL] File: {} Line: {}",
			msg, // Input file location
			loc.file_name(), loc.line() // Internal debug location
		);
	}
	else
	{
		this->msg = std::format(
			"[DEBUG INTERNAL] File: {} Line: {}",
			loc.file_name(), loc.line() // Internal debug location
		);
	}
}

EffectSyntaxException::EffectSyntaxException(const std::source_location& loc,
	const BlockAnalyzer& ctx, int block, string&& msg)
{
	const LexBlock& fxLoc = ctx.GetBlocks()[block];
	const LexFile& fxSrc = ctx.GetSourceFiles()[fxLoc.file];

	if (msg.length() > 0)
	{
		this->msg = std::format(
			"{}\nLine: {}\nFile: {}\n"
			"[DEBUG INTERNAL] File: {} Line: {}", 
			msg, fxLoc.startLine, fxSrc.filePath, // Input file location
			loc.file_name(), loc.line() // Internal debug location
		);
	}
	else
	{
		this->msg = std::format(
			"Line: {}\nFile: {}\n"
			"[DEBUG INTERNAL] File: {} Line: {}",
			fxLoc.startLine, fxSrc.filePath, // Input file location
			loc.file_name(), loc.line() // Internal debug location
		);
	}
}

EffectSyntaxException::EffectSyntaxException(const BlockAnalyzer& ctx, int block, string&& msg)
{
	const LexBlock& fxLoc = ctx.GetBlocks()[block];
	const LexFile& fxSrc = ctx.GetSourceFiles()[fxLoc.file];

	if (msg.length() > 0)
	{
		this->msg = std::format(
			"Line: {}\nFile: {}\n{}",
			fxLoc.startLine, fxSrc.filePath, msg // Input file location
		);
	}
	else
	{
		this->msg = std::format(
			"Line: {}\nFile: {}",
			fxLoc.startLine, fxSrc.filePath // Input file location
		);
	}
}