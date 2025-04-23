#include <string>
#include <string_view>

namespace Weave
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

	/// <summary>
	/// Returns a copy of a given wide UTF-16LE string as a narrow UTF-8 string.
	/// </summary>
	string GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src);

	/// <summary>
	/// Returns a copy of a given wide UTF-16LE string as a narrow UTF-8 string.
	/// </summary>
	void GetMultiByteString_UTF16LE_TO_UTF8(wstring_view src, string& dst);

	/// <summary>
	/// Returns a copy of a given narrow UTF-8 string as a UTF-16LE string.
	/// </summary>
	wstring GetWideString_UTF8_TO_UTF16LE(string_view src);

	/// <summary>
	/// Returns a copy of a given narrow UTF-8 string as a UTF-16LE string.
	/// </summary>
	void GetWideString_UTF8_TO_UTF16LE(string_view src, wstring& dst);
}