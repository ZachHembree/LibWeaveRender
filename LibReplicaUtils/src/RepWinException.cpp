#include "pch.hpp"
#include "RepWinException.hpp"
#include <winnls.h>

using namespace std;
using namespace Replica;

RepWinException::RepWinException(int line, string_view file, HRESULT hr) noexcept :
	RepException(line, file),
	hr(hr)
{ }

const char* RepWinException::what() const noexcept
{
	ostringstream ss;

	ss << GetType() << endl
		<< "Error Code: " << GetErrorCode() << endl
		<< "Description: " << GetErrorString() << endl
		<< "Line: " << line << endl
		<< "File: " << file << endl;

	whatBuf = ss.str();
	return whatBuf.c_str();
}

string RepWinException::GetErrorString() const noexcept
{
	return GetTranslatedErrorCode(hr);
}

HRESULT RepWinException::GetErrorCode() const noexcept
{
	return hr;
}

string_view RepWinException::GetType() const noexcept
{
	return "Windows Exception";
}

string RepWinException::GetTranslatedErrorCode(HRESULT hr) noexcept
{
	char* lpBuf = nullptr;
	DWORD msgLen = FormatMessageA(
		// Alloc Mem for Message + Write Address to Ptr Var | Search Sys Tables for Message
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		0, // Auto lang ID
		(LPSTR)&lpBuf,
		0,
		nullptr
	);

	if (lpBuf != nullptr)
	{
		string errString(lpBuf);
		LocalFree(lpBuf);

		return errString;
	}
	else
	{
		return "Unknown Error Code";
	}
}