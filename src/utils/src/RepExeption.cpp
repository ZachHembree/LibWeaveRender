#include "RepException.hpp"
#include <sstream>

using namespace std;
using namespace Replica;

RepException::RepException(int line, const char* file) noexcept :
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

const std::string& RepException::GetFile() const noexcept
{
	return file;
}

const char* RepException::GetType() const noexcept
{
	return "Replica Exception";
}