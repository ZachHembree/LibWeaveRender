#pragma once
#pragma warning(disable: 26444) // Intellisense is just helpless, sometimes

#include "ReplicaUtils/LeanWin.hpp"
#include "ReplicaUtils/RepException.hpp"

#define WIN_THROW_LAST() RepWinException(__LINE__, __FILE__, GetLastError())
#define WIN_THROW_HR(x) { HRESULT hr = (x); if (FAILED(hr)) { throw RepWinException(__LINE__, __FILE__, hr); } }
#define WIN_ASSERT_NZ_LAST(x) { if ((x) == 0) { WIN_THROW_LAST(); } }

namespace Replica
{
	class RepWinException : public RepException
	{
		public:

			RepWinException(int line, string_view file, HRESULT hr) noexcept;

			const char* what() const noexcept override;

			string GetErrorString() const noexcept;

			HRESULT GetErrorCode() const noexcept;

			string_view GetType() const noexcept override;
		
			static string GetTranslatedErrorCode(HRESULT hr) noexcept;

		protected:
			HRESULT hr;

	};
}