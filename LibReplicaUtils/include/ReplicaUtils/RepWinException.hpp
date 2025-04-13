#pragma once
#pragma warning(disable: 26444)

#include "ReplicaUtils/LeanWin.hpp"
#include "ReplicaUtils/RepException.hpp"

#ifndef NDEBUG
#define WIN_THROW(...) throw RepWinException(std::source_location::current(), __VA_ARGS__)
#define WIN_THROW_HR_MSG(HR, ...) throw RepWinException(std::source_location::current(), HR, __VA_ARGS__)
#else
#define WIN_THROW(...) throw RepWinException(__VA_ARGS__)
#define WIN_THROW_HR_MSG(HR, ...) throw RepWinException(HR, __VA_ARGS__)
#endif

#define WIN_THROW_HR(HR) WIN_THROW_HR_MSG(HR, "")

#define WIN_CHECK_HR_MSG(HR, ...) do { HRESULT hr = (HR); if (FAILED(hr)) { WIN_THROW_HR_MSG(hr, __VA_ARGS__); } } while(0)
#define WIN_CHECK_LAST_MSG(COND, ...) REP_CONDITION(COND, WIN_CHECK_HR_MSG(GetLastError(), __VA_ARGS__))
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
#define WIN_ASSERT_HR_MSG(HR, ...) REP_EMPTY()
#define WIN_ASSERT_LAST_MSG(COND, ...) REP_EMPTY()
#define WIN_ASSERT_NZ_LAST_MSG(X, ...) REP_EMPTY()

#define WIN_ASSERT_HR(HR) REP_EMPTY()
#define WIN_ASSERT_LAST(COND) REP_EMPTY()
#define WIN_ASSERT_NZ_LAST(X) REP_EMPTY()
#endif

namespace Replica
{
	/// <summary>
	/// Base class for Win32 exceptions
	/// </summary>
	class RepWinException : public RepException
	{
		public:
			using RepException::RepException;

			RepWinException();

			RepWinException(HRESULT hr, string&& msg = "") noexcept;

			RepWinException(const std::source_location& loc, HRESULT hr, string&& msg = "") noexcept;

			template<typename... FmtArgs>
			RepWinException(HRESULT hr, string_view fmt, FmtArgs... args) :
				RepWinException(hr, std::vformat(fmt, std::make_format_args(args...)))
			{ }

			template<typename... FmtArgs>
			RepWinException(const std::source_location& loc, HRESULT hr, string_view fmt, FmtArgs... args) :
				RepWinException(loc, hr, std::vformat(fmt, std::make_format_args(args...)))
			{ }

			HRESULT GetErrorCode() const noexcept;

			string_view GetType() const noexcept override;
		
		protected:
			HRESULT hr;
	};
}