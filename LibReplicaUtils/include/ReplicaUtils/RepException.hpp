#pragma once
#include <exception>
#include "ReplicaUtils/Utils.hpp"

#define REP_THROW() { throw RepException(__LINE__, __FILE__); }
#define REP_THROW_MSG(msg) { throw RepMsgException(__LINE__, __FILE__, msg); }
#define REP_ASSERT(x) if (!(x)) { throw RepException(__LINE__, __FILE__); }
#define REP_ASSERT_MSG(x, msg) if (!(x)) { throw RepMsgException(__LINE__, __FILE__, msg); }

namespace Replica
{
	class RepException : public std::exception
	{
	public:
		RepException(int line, string_view file) noexcept;

		const char* what() const noexcept override;

		int GetLine() const noexcept;

		const string_view& GetFile() const noexcept;

		virtual string_view GetType() const noexcept;

	protected:
		mutable std::string whatBuf;
		const int line;
		string_view file;
	};

	class RepMsgException : public RepException
	{
	public:

		RepMsgException(int line, string_view file, string_view msg) noexcept;

		const char* what() const noexcept override;

		string_view GetType() const noexcept override;

	protected:
		string_view msg;

	};
}