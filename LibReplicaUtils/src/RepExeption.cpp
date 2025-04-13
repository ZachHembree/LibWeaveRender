#include "pch.hpp"
#include "ReplicaUtils/RepException.hpp"

using namespace Replica;

RepException::RepException(string&& msg) : 
	msg(std::move(msg))
{ }

RepException::RepException(const std::source_location& loc, string&& msg)
{
	if (msg.length() > 0)
		this->msg = std::format("Line: {}\nFile: {}\nMessage: {}", loc.line(), loc.file_name(), msg);
	else
		this->msg = std::format("Line: {}\nFile: {}", loc.line(), loc.file_name());
}

string_view RepException::GetType() const noexcept 
{
	return "Replica Exception";
}

RepException::~RepException() noexcept = default;

string_view RepException::GetDescription() const noexcept { return msg; }

const char* RepException::what() const noexcept { return msg.c_str(); }
