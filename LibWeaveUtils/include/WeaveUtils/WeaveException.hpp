#pragma once
#include <exception>
#include <source_location>
#include <format>
#include <assert.h>
#include "GlobalUtils.hpp"
#include "TextUtils.hpp"

// Wraps a block in a conditional macro that behaves like a statement
#define WV_IF_NOT(COND, X) do { const bool _isCond = (COND); if (!_isCond) X; } while (0)
// Empty macro that behaves like a statement
#define WV_EMPTY(X) do { (void)(X); } while(0)

#define WV_CHECK_MSG(COND, ...) WV_IF_NOT(COND, WV_THROW(__VA_ARGS__))
#define WV_CHECK(COND) WV_CHECK_MSG(COND, "Check failed")

#ifndef NDEBUG
#define WV_THROW(...) throw WeaveException(std::source_location::current(), __VA_ARGS__)
#define WV_ASSERT(COND) WV_CHECK_MSG(COND, "Assert failed")
#define WV_ASSERT_MSG(COND, ...) WV_CHECK_MSG(COND, __VA_ARGS__)
#else
#define WV_THROW(...) throw WeaveException(__VA_ARGS__)
#define WV_ASSERT(COND) WV_EMPTY(COND)
#define WV_ASSERT_MSG(COND, ...) WV_EMPTY(COND)
#endif

namespace Weave
{
	/// <summary>
	/// Base class for all exceptions in Weave libraries
	/// </summary>
	class WeaveException : public std::exception
	{
	public:
		WeaveException(string&& msg = "");

		WeaveException(const std::source_location& loc, string&& msg = "");

		template<typename... FmtArgs>
		WeaveException(string_view fmt, FmtArgs&&... args) :
			WeaveException(std::vformat(fmt, std::make_format_args(args...)))
		{ }

		template<typename... FmtArgs>
		WeaveException(const std::source_location& loc, string_view fmt, FmtArgs&&... args) :
			WeaveException(loc, std::vformat(fmt, std::make_format_args(args...)))
		{ }

		virtual ~WeaveException() noexcept;

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
