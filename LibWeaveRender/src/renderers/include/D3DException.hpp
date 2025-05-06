#pragma once
#include "WeaveUtils/WeaveWinException.hpp"

#ifndef NDEBUG
#define D3D_THROW(...) throw D3DException(std::source_location::current(), __VA_ARGS__)
#define D3D_THROW_HR_MSG(HR, ...) throw D3DException(std::source_location::current(), HR, __VA_ARGS__)
#else
#define D3D_THROW(...) throw D3DException(__VA_ARGS__)
#define D3D_THROW_HR_MSG(HR, ...) throw D3DException(HR, __VA_ARGS__)
#endif // !NDEBUG

#define D3D_CHECK_MSG(COND, ...) WV_IF_NOT( COND, D3D_THROW(__VA_ARGS__) )
#define D3D_CHECK(COND) WV_IF_NOT( COND, D3D_THROW("Check failed") )
#define D3D_CHECK_HR_MSG(HR, ...) do { slong hr = (HR); if (FAILED(hr)) { D3D_THROW_HR_MSG(hr, __VA_ARGS__); } } while(0)
#define D3D_CHECK_HR(HR) D3D_CHECK_HR_MSG(HR, "")

#ifndef NDEBUG
#define D3D_ASSERT_MSG(COND, ...) D3D_CHECK_MSG(COND, __VA_ARGS__)
#define D3D_ASSERT(COND) D3D_CHECK_MSG(COND, "Assert failed")
#define D3D_ASSERT_HR_MSG(HR, ...) D3D_CHECK_HR_MSG(HR, __VA_ARGS__)
#define D3D_ASSERT_HR(HR) D3D_CHECK_HR_MSG(HR, "")
#else
#define D3D_ASSERT_MSG(COND, ...) WV_EMPTY(COND)
#define D3D_ASSERT(COND) WV_EMPTY(COND)
#define D3D_ASSERT_HR_MSG(HR, ...) WV_EMPTY(HR)
#define D3D_ASSERT_HR(HR) WV_EMPTY(HR)
#endif // !NDEBUG

namespace Weave::D3D11
{
	/// <summary>
	/// Base class for all D3D exceptions
	/// </summary>
	class D3DException : public WeaveWinException
	{
	public:
		using WeaveWinException::WeaveWinException;

		template<typename... FmtArgs>
		D3DException(slong hr, string_view fmt, FmtArgs... args) :
			D3DException(hr, std::vformat(fmt, std::make_format_args(args...)))
		{ }

		template<typename... FmtArgs>
		D3DException(const std::source_location& loc, slong hr, string_view fmt, FmtArgs... args) :
			D3DException(loc, hr, std::vformat(fmt, std::make_format_args(args...)))
		{ }

		string_view GetType() const noexcept override;
	};
}
