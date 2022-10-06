#pragma once
#include "RepWinException.hpp"

#define GFX_ASSERT(x, msg) { if (!(x)) throw GfxAssertException(__LINE__, __FILE__, msg); }
#define GFX_THROW_FAILED(x) { HRESULT hr = (x); if (FAILED(hr)) { throw GfxException(__LINE__, __FILE__, hr); } }
#define GFX_THROW(msg) throw GfxAssertException(__LINE__, __FILE__, msg);

namespace Replica::D3D11
{
	class GfxException : public RepWinException
	{
	public:
		GfxException(int line, const char* file, HRESULT hr) noexcept;

		string_view GetType() const noexcept override;
	};

	class GfxAssertException : public RepException
	{
	public:
		GfxAssertException(int line, const char* file, const char* msg) noexcept;

		const char* what() const noexcept override;

		string_view GetType() const noexcept override;

	private:
		const char* msg;
	};
}
