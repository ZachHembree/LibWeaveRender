#pragma once
#pragma warning(disable: 26444)

#include "RepLeanWin.h"
#include "RepException.hpp"

#define REP_EXCEPT(hr) RepWinException(__LINE__, __FILE__, hr)
#define REP_EXCEPT_LAST() RepWinException(__LINE__, __FILE__, GetLastError())
#define THROW_FAILED(x) { HRESULT hr = (x); if (FAILED(hr)) { throw RepWinException(__LINE__, __FILE__, hr); } }
#define WIN_ASSERT_NZ_LAST(TYPE, x) { const TYPE ret = (x); if (ret == 0) { REP_EXCEPT_LAST(); } }

namespace Replica
{
	class RepWinException : public RepException
	{
		public:

			RepWinException(int line, const char* file, HRESULT hr) noexcept;

			const char* what() const noexcept override;
			std::string GetErrorString() const noexcept;
			HRESULT GetErrorCode() const noexcept;
			const char* GetType() const noexcept override;
		
			static std::string GetTranslatedErrorCode(HRESULT hr) noexcept;

		protected:
			HRESULT hr;

	};
}