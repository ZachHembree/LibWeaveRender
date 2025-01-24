#include "pch.hpp"
#include <sstream>
#include "../src/utils/include/GfxException.hpp"

using namespace std;
using namespace Replica;
using namespace Replica::D3D11;

GfxException::GfxException(int line, const char* file, HRESULT hr) noexcept :
	RepWinException(line, file, hr)
{ }

string_view GfxException::GetType() const noexcept
{
	return "D3D11 Exception";
}

GfxAssertException::GfxAssertException(int line, const char* file, const char* msg) noexcept :
	RepException(line, file),
	msg(msg)
{ }

const char* GfxAssertException::what() const noexcept
{
	std::ostringstream ss;

	ss << GetType() << std::endl
		<< "Message: " << msg << std::endl
		<< "Line: " << line << std::endl
		<< "File: " << file << std::endl;

	whatBuf = ss.str();
	return whatBuf.c_str();
}

string_view GfxAssertException::GetType() const noexcept
{
	return "General GFX";
}