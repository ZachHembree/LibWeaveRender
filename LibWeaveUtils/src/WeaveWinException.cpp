#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "WeaveUtils/WeaveWinException.hpp"

using namespace std;
using namespace Weave;

static string GetTranslatedErrorCode(slong hr) noexcept
{
	wchar_t* lpBuf = nullptr;
	DWORD msgLen = FormatMessageW(
		// Alloc Mem for Message + Write Address to Ptr Var | Search Sys Tables for Message
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		(DWORD)hr,
		0, // Auto lang ID
		(wchar_t*)&lpBuf,
		0,
		nullptr
	);

	if (lpBuf != nullptr)
	{
		string errString = GetMultiByteString_UTF16LE_TO_UTF8(wstring_view(lpBuf));
		LocalFree(lpBuf);
		return errString;
	}
	else
		return "Unknown Error Code";
}

WeaveWinException::WeaveWinException() :
	hr(E_FAIL)
{ }

WeaveWinException::WeaveWinException(slong hr, string&& msg) noexcept :
	hr(hr)
{
	if (msg.length() > 0)
	{
		this->msg = std::format(
			"Error Code: {}\nDescription: {}\nMessage: {}\n",
			hr, GetTranslatedErrorCode(hr), msg
		);
	}
	else
	{
		this->msg = std::format(
			"Error Code: {}\nDescription: {}\n",
			hr, GetTranslatedErrorCode(hr)
		);
	}
}

WeaveWinException::WeaveWinException(const std::source_location& loc, slong hr, string&& msg) noexcept :
	hr(hr)
{ 
	if (msg.length() > 0)
	{
		this->msg = std::format(
			"Error Code: {}\nDescription: {}\nLine: {}\nFile: {}\nMessage: {}\n",
			hr, GetTranslatedErrorCode(hr), loc.line(), loc.file_name(), msg
		);
	}
	else
	{
		this->msg = std::format(
			"Error Code: {}\nDescription: {}\nLine: {}\nFile: {}\n",
			hr, GetTranslatedErrorCode(hr), loc.line(), loc.file_name()
		);
	}
}

slong WeaveWinException::GetErrorCode() const noexcept { return hr; }

string_view WeaveWinException::GetType() const noexcept { return "Windows Exception"; }