#pragma once

#include <exception>
#include <string>

namespace Replica
{
	class RepException : public std::exception
	{
	public:
		RepException(int line, const char* file) noexcept;

		const char* what() const noexcept override;
		int GetLine() const noexcept;
		const std::string& GetFile() const noexcept;
		virtual const char* GetType() const noexcept;

	protected:
		mutable std::string whatBuf;
		const int line;
		const std::string file;
	};
}