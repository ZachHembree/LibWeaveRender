#include "../src/utils/include/RepException.hpp"
#include <sstream>

using namespace std;
using namespace Replica;

RepException::RepException(int line, string_view file) noexcept :
	line(line),
	file(file)
{ }

const char* RepException::what() const noexcept
{
	ostringstream ss;

	ss << GetType() << endl
		<< "Line: " << line << endl
		<< "File: " << file << endl;

	whatBuf = ss.str();
	return whatBuf.c_str();
}

int RepException::GetLine() const noexcept
{
	return line;
}

const std::string_view& RepException::GetFile() const noexcept
{
	return file;
}

string_view RepException::GetType() const noexcept
{
	return "Replica Exception";
}

RepMsgException::RepMsgException(int line, const char* file, string_view msg) noexcept :
	RepException(line, file),
	msg(msg)
{ }

const char* RepMsgException::what() const noexcept
{
	ostringstream ss;

	ss << GetType() << endl
		<< "Line: " << line << endl
		<< "File: " << file << endl
		<< "Message: " << msg;

	whatBuf = ss.str();
	return whatBuf.c_str();
}

string_view RepMsgException::GetType() const noexcept
{
	return "Rep Msg Exception";
}
