#pragma once
#include "ReplicaUtils/RepException.hpp"

#ifndef NDEBUG

// Throw with source location for debug
#define FX_THROW(...) throw EffectParseException(std::source_location::current(), __VA_ARGS__)
#define FXSYNTAX_THROW(...) throw EffectSyntaxException(std::source_location::current(), __VA_ARGS__)
#define FXBLOCK_THROW(CTX, BLOCK, ...) throw EffectSyntaxException(std::source_location::current(), CTX, BLOCK, __VA_ARGS__)

#define FX_ASSERT(COND) REP_IF_NOT(COND, FX_THROW("Assert failed") )
#define FX_ASSERT_MSG(COND, ...) REP_IF_NOT(COND, FX_THROW(__VA_ARGS__) )

#define FXSYNTAX_ASSERT(COND) REP_IF_NOT(COND, FXSYNTAX_THROW("Assert failed") )
#define FXSYNTAX_ASSERT_MSG(COND, ...) REP_IF_NOT(COND, FXSYNTAX_THROW(__VA_ARGS__) )

#define FXBLOCK_ASSERT(COND,CTX, BLOCK) REP_IF_NOT(COND, FXBLOCK_THROW(CTX, BLOCK, "Assert failed") )
#define FXBLOCK_ASSERT_MSG(COND, CTX, BLOCK, ...) REP_IF_NOT(COND, FXBLOCK_THROW(CTX, BLOCK, __VA_ARGS__) )

#else

// Throw without source location for release
#define FX_THROW(...) throw ParseException(__VA_ARGS__)
#define FXSYNTAX_THROW(...) throw ParseSyntaxException(__VA_ARGS__)
#define FXBLOCK_THROW(CTX, BLOCK, ...) throw ParseSyntaxException(CTX, BLOCK, __VA_ARGS__)

#define FX_ASSERT(COND) REP_EMPTY(COND)
#define FX_ASSERT_MSG(COND, ...) REP_EMPTY(COND)

#define FXSYNTAX_ASSERT(COND) REP_EMPTY(COND)
#define FXSYNTAX_ASSERT_MSG(COND, ...) REP_EMPTY(COND)

#define FXBLOCK_ASSERT(COND,CTX, BLOCK) REP_EMPTY(COND)
#define FXBLOCK_ASSERT_MSG(COND, CTX, BLOCK, ...) REP_EMPTY(COND)

#endif

#define FX_CHECK(COND) REP_IF_NOT(COND, FX_THROW("Check failed") )
#define FX_CHECK_MSG(COND, ...) REP_IF_NOT(COND, FX_THROW(__VA_ARGS__) )

#define FXSYNTAX_CHECK(COND) REP_IF_NOT(COND, FXSYNTAX_THROW("Check failed") )
#define FXSYNTAX_CHECK_MSG(COND, ...) REP_IF_NOT(COND, FXSYNTAX_THROW(__VA_ARGS__) )

#define FXBLOCK_CHECK(COND,CTX, BLOCK) REP_IF_NOT(COND, FXBLOCK_THROW(CTX, BLOCK, "Check failed") )
#define FXBLOCK_CHECK_MSG(COND, CTX, BLOCK, ...) REP_IF_NOT(COND, FXBLOCK_THROW(CTX, BLOCK, __VA_ARGS__) )

namespace Replica::Effects
{
	class BlockAnalyzer;

	/// <summary>
	/// Base class for all exceptions raised in the effect parser
	/// </summary>
	class EffectParseException : public RepException
	{
	public:
		using RepException::RepException;

		string_view GetType() const noexcept override;
	};

	/// <summary>
	/// Base class for all exceptions raised for syntax errors in parsed input. Automatically includes 
	/// location in source file.
	/// </summary>
	class EffectSyntaxException : public EffectParseException
	{
	public:
		using EffectParseException::EffectParseException;

		EffectSyntaxException(const std::source_location& loc, const BlockAnalyzer& ctx, int block, string&& msg);

		EffectSyntaxException(const BlockAnalyzer& ctx, int block, string&& msg);

		template<typename... FmtArgs>
		EffectSyntaxException(const BlockAnalyzer& ctx, int block, string_view fmt, FmtArgs... args) :
			EffectSyntaxException(std::vformat(fmt, std::make_format_args(args...)))
		{ }

		template<typename... FmtArgs>
		EffectSyntaxException(const std::source_location& loc, const BlockAnalyzer& ctx, int block, string_view fmt, FmtArgs... args) :
			EffectSyntaxException(loc, std::vformat(fmt, std::make_format_args(args...)))
		{ }

	protected:
		string parseMsg;
	};
}