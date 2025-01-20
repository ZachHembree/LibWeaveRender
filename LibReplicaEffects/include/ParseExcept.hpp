#pragma once
#include "boost/wave/cpp_exceptions.hpp"
#include "RepException.hpp"
#include <format>
#include <sstream>

#define PARSE_ASSERT(x) { if (!(x)) { throw FxParseException(__LINE__, __FILE__); } }
#define PARSE_ASSERT_MSG(x, errMsg) { if (!(x)) { throw FxParseException(__LINE__, __FILE__, errMsg); } }

#define PARSE_ASSERT_LEN(x)  { if (!(x)) { throw FxParseException(__LINE__, __FILE__, "Unexpected expression ending."); } }
#define PARSE_ERR(errMsg) { throw FxParseException(__LINE__, __FILE__, errMsg); }
#define PARSE_ERR_FMT(fmt, ...) \
	{ \
		string msg = std::format(fmt, __VA_ARGS__); \
		throw FxParseException(__LINE__, __FILE__, msg.data()); \
	}
#define PARSE_ASSERT_FMT(x, fmt, ...) \
	{ \
		if (!(x)) \
		{\
			string msg = std::format(fmt, __VA_ARGS__); \
			throw FxParseException(__LINE__, __FILE__, msg.data()); \
		}\
	}

namespace Replica::Effects
{
	/// <summary>
	/// Base class for exceptions thrown by Wave
	/// </summary>
	using WaveException = boost::wave::cpp_exception;

	class FxParseException : public WaveException
	{
	public:
		FxParseException(std::size_t line, char const* filename, string_view msg = "") noexcept :
			WaveException(line, 0, filename)
		{ 
			if (msg.length() > 0)
				desc = std::format("Line: {}\nFile: {}\nMessage: {}", line, filename, msg);
			else
				desc = std::format("Line: {}\nFile: {}", line, filename);
		}

		char const* what() const noexcept override
		{
			return "FX Parse Exception";
		}

		virtual char const* description() const noexcept override
		{
			return desc.c_str();
		}

		int get_errorcode() const noexcept override { return 4; }

		int get_severity() const noexcept override { return 4; }

		char const* get_related_name() const throw() { return "<unknown>"; }

		bool is_recoverable() const noexcept override { return false; }

	protected:
		string desc;
	};
}