#pragma once
#include <string>
#include <ostream>
#include <string_view>
#include "ReplicaUtils/Span.hpp"

namespace Replica
{ 
    /// <summary>
    /// Span type used to represent a range of characters within a string. Does not
    /// store text.
    /// </summary>
    class TextBlock : public Span<const char>
    {
    using string = std::string;
    using string_view = std::string_view;

    public:
        using Span<const char>::GetLength;
        using Span<const char>::operator[];
        using Span<const char>::GetPtr;
        using Span<const char>::begin;
        using Span<const char>::end;

        TextBlock();

        TextBlock(const char* pStart);

        TextBlock(const char* pStart, size_t length);

        TextBlock(const char* pStart, const char* pEnd);

        TextBlock(string& str);

        TextBlock(string_view& str);

        TextBlock(Span<char>& span);

        /// <summary>
        /// Returns true if the given substring is present at or after the given start.
        /// </summary>
        bool Contains(const string_view& substr, const char* pStart = nullptr) const
        {
            return Find(substr, pStart) != nullptr;
        }

        /// <summary>
        /// Returns true if the given substring is present at or after the given start.
        /// </summary>
        bool Contains(const char* substr, int subLen, const char* pStart = nullptr) const
        {
            return Find(substr, subLen, pStart) != nullptr;
        }

        /// <summary>
        /// Returns the total number of the given character in the text block, starting
        /// from the given point.
        /// </summary>
        int FindCount(const char ch, const char* pStart = nullptr) const;

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const string_view& substr, const char* pStart = nullptr) const
        {
            return Find(&substr[0], (int)substr.length(), pStart);
        }

        //// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const TextBlock& substr, const char* pStart = nullptr) const
        {
            return Find(substr.pStart, (int)substr.length, pStart);
        }

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const char ch, const char* pStart = nullptr) const
        {
            return Find(&ch, 1, pStart);
        }

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const char* substr, int subLen, const char* pStart = nullptr) const;

        /// <summary>
        /// Returns pointer to first character in a word found in the text after the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindWord(const char* pStart, const string_view& breakFilter = "") const
        {
            return FindStart(pStart, breakFilter, '!', '~');
        }

        /// <summary>
        /// Returns pointer to last character in a word found in the text after the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindWordEnd(const char* pStart, const string_view& breakFilter = "") const
        {
            return FindEnd(pStart, breakFilter, '!', '~');
        }

        /// <summary>
        /// Returns pointer to first character in a word found in the text before the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindLastWord(const char* pStart, const string_view& breakFilter = "") const
        {
            return FindLastStart(pStart, breakFilter, '!', '~');
        }

        TextBlock GetWord(const char* pStart, const string_view& breakFilter = "", char min = '!', char max = '~') const
        {
            pStart = FindStart(pStart, breakFilter, min, max);
            const char* pEnd = FindEnd(pStart, breakFilter, min, max);
            return TextBlock(pStart, pEnd);
        }

        TextBlock GetLastWord(const char* pStart, const string_view& breakFilter = "", char min = '!', char max = '~') const
        {
            pStart = FindLastStart(pStart, breakFilter, min, max);
            const char* pEnd = FindEnd(pStart, breakFilter, min, max);
            return TextBlock(pStart, pEnd);
        }

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const string_view& substr, const char* pStart = nullptr);

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const char* substr, int subLen, const char* pStart = nullptr);

        /// <summary>
        /// Returns pointer to first character in a range found in the text after the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindStart(const char* pStart, const string_view& breakFilter = "", char min = '\0', char max = (char)127) const;

        /// <summary>
        /// Returns pointer to first character in a range found in the text after the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindEnd(const char* pStart, const string_view& breakFilter = "", char min = '\0', char max = (char)127) const;

        /// <summary>
        /// Returns pointer to first character in a range found in the text before the 
        /// given start, with the given bounding characters.
        /// </summary>
        const char* FindLastStart(const char* pStart, const string_view& breakFilter = "", char min = '\0', char max = (char)127) const;

        /// <summary>
        /// Finds position of the first character of the first matching occurence of 
        /// the given substring, starting from the given pointer. Doesn't stop on '\0'.
        /// </summary>
        const char* Find(const char ch, const char* pStart = nullptr)
        {
            return Find(&ch, 1, pStart);
        }

        /// <summary>
        /// Writes a null-terminated copy of the text span's contents to the given buffer.
        /// </summary>
        template<size_t size>
        void CopyTo(char(&arr)[size]) const { CopyTo(arr, size); }

        /// <summary>
        /// Writes a null-terminated copy of the text span's contents to the given buffer.
        /// </summary>
        void CopyTo(string& buf) const { CopyTo(&buf[0], buf.length()); }

        /// <summary>
        /// Writes a null-terminated copy of the text span's contents to the given buffer.
        /// </summary>
        void CopyTo(char* dst, size_t maxLength)  const;

        /// <summary>
        /// Returns a copy of the range as a new string
        /// </summary>
        string ToString()
        {
            return string(pStart, length);
        }

        /// <summary>
        /// Returns a copy of the range as a new string
        /// </summary>
        static string ToString(const char* pStart, const char* pEnd)
        {
            size_t len = UnsignedDelta(pEnd, pStart);
            return string(pStart, len);
        }

        /// <summary>
        /// Returns true if the given character falls inside the given range and isn't in the given string filter.
        /// </summary>
        static bool GetIsRangeChar(char ch, const string_view& filter = "", char min = '\0', char max = (char)127);

        /// <summary>
        /// Returns a string view interface representing the range
        /// </summary>
        operator string_view() const { return string_view(pStart, length); }

        friend std::ostream& operator<<(std::ostream& os, const TextBlock& text)
        {
            return os.write(text.pStart, text.length);
        }
    };
}