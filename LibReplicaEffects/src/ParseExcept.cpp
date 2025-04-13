#include "pch.hpp"
#include "ReplicaEffects/ParseExcept.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderParser/BlockAnalyzer.hpp"

using namespace Replica;
using namespace Replica::Effects;

string_view ParseException::GetType() const noexcept
{
	return "FX Parse Error";
}

ParseSyntaxException::ParseSyntaxException(const std::source_location& loc,
	const BlockAnalyzer& ctx, int block, string&& msg)
{
	const LexBlock& fxLoc = ctx.GetBlocks()[block];
	const LexFile& fxSrc = ctx.GetSourceFiles()[fxLoc.file];

	if (msg.length() > 0)
	{
		this->msg = std::format(
			"Line: {}\nFile: {}\n{}\n"
			"[DEBUG INTERNAL] Line: {}\nFile: {}", 
			fxLoc.startLine, fxSrc.filePath, msg, // Input file location
			loc.line(), loc.file_name() // Internal debug location
		);
	}
	else
	{
		this->msg = std::format(
			"Line: {}\nFile: {}\n"
			"[DEBUG INTERNAL] Line: {}\nFile: {}",
			fxLoc.startLine, fxSrc.filePath, // Input file location
			loc.line(), loc.file_name() // Internal debug location
		);
	}
}

ParseSyntaxException::ParseSyntaxException(const BlockAnalyzer& ctx, int block, string&& msg)
{
	const LexBlock& fxLoc = ctx.GetBlocks()[block];
	const LexFile& fxSrc = ctx.GetSourceFiles()[fxLoc.file];

	if (msg.length() > 0)
		this->msg = std::format("Line: {}\nFile: {}\n{}", fxLoc.startLine, fxSrc.filePath, msg);
	else
		this->msg = std::format("Line: {}\nFile: {}", fxLoc.startLine, fxSrc.filePath);
}