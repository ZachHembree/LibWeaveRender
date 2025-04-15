#include "pch.hpp"
#include "WeaveUtils/WeaveException.hpp"

using namespace Weave;

WeaveException::WeaveException(string&& msg) : 
	msg(std::move(msg))
{ }

WeaveException::WeaveException(const std::source_location& loc, string&& msg)
{
	if (msg.length() > 0)
		this->msg = std::format("Line: {}\nFile: {}\nMessage: {}", loc.line(), loc.file_name(), msg);
	else
		this->msg = std::format("Line: {}\nFile: {}", loc.line(), loc.file_name());
}

string_view WeaveException::GetType() const noexcept 
{
	return "Weave Exception";
}

WeaveException::~WeaveException() noexcept = default;

string_view WeaveException::GetDescription() const noexcept { return msg; }

const char* WeaveException::what() const noexcept { return msg.c_str(); }
