#include "pch.hpp"
#include "ReplicaUtils/RepWinException.hpp"
#include <winnls.h>

using namespace std;
using namespace Replica;

static string GetTranslatedErrorCode(HRESULT hr) noexcept
{
	wchar_t* lpBuf = nullptr;
	DWORD msgLen = FormatMessageW(
		// Alloc Mem for Message + Write Address to Ptr Var | Search Sys Tables for Message
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
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

RepWinException::RepWinException() :
	hr(E_FAIL)
{ }

RepWinException::RepWinException(HRESULT hr, string&& msg) noexcept :
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

RepWinException::RepWinException(const std::source_location& loc, HRESULT hr, string&& msg) noexcept :
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

HRESULT RepWinException::GetErrorCode() const noexcept { return hr; }

string_view RepWinException::GetType() const noexcept { return "Windows Exception"; }