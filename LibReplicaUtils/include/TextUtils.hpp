#include <string>
#include <string_view>

namespace Replica
{
	using std::string_view;
	using std::wstring_view;
	using std::string;
	using std::wstring;

	/// <summary>
	/// Appends a c-style formatted string to the given string
	/// </summary>
	void GetFmtString(string& text, const char* fmt, ...);

	/// <summary>
	/// Returns a c-style formatted string as a std::string
	/// </summary>
	string GetFmtString(const char* fmt, ...);
}