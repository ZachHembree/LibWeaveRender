#pragma once
#include <exception>
#include <source_location>
#include <format>
#include "ReplicaUtils/TextUtils.hpp"

// Wraps a block in a conditional macro that behaves like a statement
#define REP_IF_NOT(COND, X) do { const bool _isCond = (COND); if (!_isCond) X; } while (0)
// Empty macro that behaves like a statement
#define REP_EMPTY(X) do { (void)(X); } while(0)

#define REP_CHECK_MSG(COND, ...) REP_IF_NOT(COND,  REP_THROW(__VA_ARGS__) )
#define REP_CHECK(COND) REP_CHECK_MSG(COND, "Check failed")

#ifndef NDEBUG

#define REP_THROW(...) throw RepException(std::source_location::current(), __VA_ARGS__)
#define REP_ASSERT(COND) REP_CHECK_MSG(COND, "Assert failed")
#define REP_ASSERT_MSG(COND, ...) REP_CHECK_MSG(COND, __VA_ARGS__)

#else

#define REP_THROW(...) throw RepException(__VA_ARGS__)
#define REP_ASSERT(COND) REP_EMPTY(COND)
#define REP_ASSERT_MSG(COND, ...) REP_EMPTY(COND)

#endif

namespace Replica
{
	/// <summary>
	/// Base class for all exceptions in Replica libraries
	/// </summary>
	class RepException : public std::exception
	{
	public:
		RepException(string&& msg = "");

		RepException(const std::source_location& loc, string&& msg = "");

		template<typename... FmtArgs>
		RepException(string_view fmt, FmtArgs&&... args) :
			RepException(std::vformat(fmt, std::make_format_args(args...)))
		{ }

		template<typename... FmtArgs>
		RepException(const std::source_location& loc, string_view fmt, FmtArgs&&... args) :
			RepException(loc, std::vformat(fmt, std::make_format_args(args...)))
		{ }

		virtual ~RepException() noexcept;

		/// <summary>
		/// Returns string describing the type of exception
		/// </summary>
		virtual string_view GetType() const noexcept;

		/// <summary>
		/// Returns string describing error details
		/// </summary>
		virtual string_view GetDescription() const noexcept;

		/// <summary>
		/// Returns string describing error details
		/// </summary>
		const char* what() const noexcept override;

	protected:
		string msg;
	};
}