#pragma once
#pragma warning(disable: 26444)

#include "WeaveUtils/LeanWin.hpp"
#include "WeaveUtils/WeaveException.hpp"

#ifndef NDEBUG
#define WIN_THROW(...) throw WeaveWinException(std::source_location::current(), __VA_ARGS__)
#define WIN_THROW_HR_MSG(HR, ...) throw WeaveWinException(std::source_location::current(), HR, __VA_ARGS__)
#else
#define WIN_THROW(...) throw WeaveWinException(__VA_ARGS__)
#define WIN_THROW_HR_MSG(HR, ...) throw WeaveWinException(HR, __VA_ARGS__)
#endif

#define WIN_THROW_HR(HR) WIN_THROW_HR_MSG(HR, "")

#define WIN_CHECK_HR_MSG(HR, ...) do { HRESULT _hr = (HR); if (FAILED(_hr)) { WIN_THROW_HR_MSG(_hr, __VA_ARGS__); } } while(0)
#define WIN_CHECK_LAST_MSG(COND, ...) WV_IF_NOT(COND, WIN_CHECK_HR_MSG(GetLastError(), __VA_ARGS__))
#define WIN_CHECK_NZ_LAST_MSG(X, ...) WIN_CHECK_LAST_MSG((X) != 0, __VA_ARGS__)

#define WIN_CHECK_HR(HR) WIN_CHECK_HR_MSG(HR, "Check failed")
#define WIN_CHECK_LAST(COND) WIN_CHECK_LAST_MSG(COND, "Check failed")
#define WIN_CHECK_NZ_LAST(X) WIN_CHECK_NZ_LAST_MSG(X, "Check failed")

#ifndef NDEBUG
#define WIN_ASSERT_HR_MSG(HR, ...) WIN_CHECK_HR_MSG(HR, __VA_ARGS__)
#define WIN_ASSERT_LAST_MSG(COND, ...) WIN_CHECK_LAST_MSG(COND, __VA_ARGS__)
#define WIN_ASSERT_NZ_LAST_MSG(X, ...) WIN_CHECK_NZ_LAST_MSG(X, __VA_ARGS__)

#define WIN_ASSERT_HR(HR) WIN_CHECK_HR_MSG(HR, "Assert failed")
#define WIN_ASSERT_LAST(COND) WIN_CHECK_LAST_MSG(COND, "Assert failed")
#define WIN_ASSERT_NZ_LAST(X) WIN_CHECK_NZ_LAST_MSG(X, "Assert failed")
#else
#define WIN_ASSERT_HR_MSG(HR, ...) WV_EMPTY(HR)
#define WIN_ASSERT_LAST_MSG(COND, ...) WV_EMPTY(COND)
#define WIN_ASSERT_NZ_LAST_MSG(X, ...) WV_EMPTY(X)

#define WIN_ASSERT_HR(HR) WV_EMPTY(HR)
#define WIN_ASSERT_LAST(COND) WV_EMPTY(COND)
#define WIN_ASSERT_NZ_LAST(X) WV_EMPTY(X)
#endif

namespace Weave
{
	/// <summary>
	/// Base class for Win32 exceptions
	/// </summary>
	class WeaveWinException : public WeaveException
	{
		public:
			using WeaveException::WeaveException;

			WeaveWinException();

			WeaveWinException(HRESULT hr, string&& msg = "") noexcept;

			WeaveWinException(const std::source_location& loc, HRESULT hr, string&& msg = "") noexcept;

			template<typename... FmtArgs>
			WeaveWinException(HRESULT hr, string_view fmt, FmtArgs... args) :
				WeaveWinException(hr, std::vformat(fmt, std::make_format_args(args...)))
			{ }

			template<typename... FmtArgs>
			WeaveWinException(const std::source_location& loc, HRESULT hr, string_view fmt, FmtArgs... args) :
				WeaveWinException(loc, hr, std::vformat(fmt, std::make_format_args(args...)))
			{ }

			HRESULT GetErrorCode() const noexcept;

			string_view GetType() const noexcept override;
		
		protected:
			HRESULT hr;
	};
}