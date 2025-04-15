#pragma once
#include <ostream>
#include "VectorSpan.hpp"

namespace Weave
{
	/// <summary>
	/// Reallocation tolerant span using std::string. Non-owning. Previously obtained iterators and pointers 
	/// may become invalid after reallocation, but span accessors provide updated pointers.
	/// </summary>
	class StringSpan : public VectorSpan<std::string>
	{
	public:
		DEF_DYN_ARR_TRAITS(VectorSpan<std::string>)
		using string = std::string;
		using string_view = std::string_view;
		using VectorSpan<string>::VectorSpan;

		/// <summary>
		/// Returns pointer to first character in the span. Requires null-termination of substring.
		/// </summary>
		char* c_str() { assert(*GetLast() == '\0'); return GetFirst(); }

		/// <summary>
		/// Returns pointer to first character in the span. Requires null-termination of substring.
		/// </summary>
		const char* c_str() const { assert(*GetLast() == '\0'); return GetFirst(); }

		/// <summary>
		/// Returns pointer to first character in the span. Does not guarantee null termination.
		/// </summary>
		char* data() { return GetFirst(); }

		/// <summary>
		/// Returns pointer to first character in the span. Does not guarantee null termination.
		/// </summary>
		const char* data() const { return GetFirst(); }

		/// <summary>
		/// Returns a string view interface representing the range
		/// </summary>
		operator string_view() const { return string_view(GetFirst(), length); }

		/// <summary>
		/// Writes substring to output stream
		/// </summary>
		friend std::ostream& operator<<(std::ostream& os, const StringSpan& text)
		{
			return os.write(text.GetFirst(), text.GetLength());
		}
	};
}

namespace std
{
	template<>
	struct hash<Weave::StringSpan>
	{
		size_t operator()(const Weave::StringSpan& arr) const noexcept { return arr.GetHash(); }
	};
}