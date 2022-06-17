#pragma once
#include "RepWindow.hpp"
#include "RepException.hpp"

#define REP_EXCEPT(hr) RepWinException(__LINE__, __FILE__, hr)
#define REP_EXCEPT_LAST() RepWinException(__LINE__, __FILE__, GetLastError())

class RepWinException : public RepException
{
	public:

		RepWinException(int line, const char* file, HRESULT hr) noexcept;

		const char* what() const noexcept override;
		std::string GetErrorString() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		const char* GetType() const noexcept override;

		static std::string GetTranslatedErrorCode(HRESULT hr) noexcept;

	private:
		HRESULT hr;

};