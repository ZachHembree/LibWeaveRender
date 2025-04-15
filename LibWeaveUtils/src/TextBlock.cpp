#include "pch.hpp"
#include "WeaveUtils/TextBlock.hpp"

using namespace Weave;

char* ClampPtr(char* ptr, char* min, char* max)
{
    ptr = ptr >= min ? ptr : min;
    ptr = ptr <= max ? ptr : max;
    return ptr;
}

const char* ClampPtr(const char* ptr, const char* min, const char* max)
{
    ptr = ptr >= min ? ptr : min;
    ptr = ptr <= max ? ptr : max;
    return ptr;
}

TextBlock::TextBlock() : Span()
{ }

TextBlock::TextBlock(const char* pStart, size_t length) : Span(pStart, length)
{
    if (length > 0 && GetBack() == '\0')
        this->length--;
}

TextBlock::TextBlock(const char* pStart, const char* pEnd) : 
    TextBlock(pStart, UnsignedDelta(pEnd + 1, pStart))
{ }

TextBlock::TextBlock(const char* pStart) : TextBlock(pStart, 1)
{ }

TextBlock::TextBlock(string& str) : TextBlock(&str[0], str.length())
{ }

TextBlock::TextBlock(string_view& str) : TextBlock(&str[0], str.length())
{ }

TextBlock::TextBlock(Span<char>& span) : TextBlock(span.GetData(), span.GetLength())
{ }

/// <summary>
/// Returns true if the given substring is present at or after the given start.
/// </summary>
bool TextBlock::StartsWith(const string_view & substr) const { return StartsWith(substr.data(), substr.size()); }

/// <summary>
/// Returns true if the given substring is present at or after the given start.
/// </summary>
bool TextBlock::StartsWith(const char* substr, size_t subLen) const 
{
    if (subLen <= length)
        return memcmp(substr, this->pStart, subLen) == 0;
    else
        return false;
}

/// <summary>
/// Returns true if the given substring is present at or after the given start.
/// </summary>
bool TextBlock::Contains(const string_view & substr, const char* pStart) const
{
    return Find(substr, pStart) != nullptr;
}

/// <summary>
/// Returns true if the given substring is present at or after the given start.
/// </summary>
bool TextBlock::Contains(const char* substr, size_t subLen, const char* pStart) const
{
    return Find(substr, subLen, pStart) != nullptr;
}

/// <summary>
/// Returns the total number of the given character in the text block, starting
/// from the given point.
/// </summary>
int TextBlock::FindCount(const char ch, const char* pStart) const
{
    int count = 0;

    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(&GetBack(), pStart);

    if (pStart >= GetData() && remLen >= 1)
    {
        for (size_t i = 0; i <= remLen; i++)
        {
            if (pStart[i] == ch)
            {
                count++;
            }
        }
    }

    return count;
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const string_view& substr, const char* pStart) const
{
    return Find(&substr[0], (int)substr.length(), pStart);
}

//// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const TextBlock& substr, const char* pStart) const
{
    return Find(substr.pStart, (int)substr.length, pStart);
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const char ch, const char* pStart) const
{
    return Find(&ch, 1, pStart);
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const string_view& substr, const char* pStart)
{
    return Find(&substr[0], (int)substr.length(), pStart);
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const char* substr, size_t subLen, const char* pStart) const
{
    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(&GetBack(), pStart);

    if (substr[subLen - 1] == '\0')
        subLen--;

    if (pStart >= GetData() && remLen >= subLen)
    {
        for (size_t i = 0; i < remLen; i++)
        {
            if (pStart[i] == substr[0])
            {
                size_t matchLen = 1;

                for (size_t j = 1; j < subLen; j++)
                {
                    if (pStart[i + j] != substr[j])
                        break;

                    matchLen++;
                }

                if (matchLen == subLen)
                    return &pStart[i];
            }
        }
    }

    return nullptr;
}

/// <summary>
/// Returns pointer to first character in a word found in the text after the 
/// given start, with the given bounding characters.
/// </summary>
const char* TextBlock::FindWord(const char* pStart, const string_view& breakFilter) const
{
    return FindStart(pStart, breakFilter, '!', '~');
}

/// <summary>
/// Returns pointer to last character in a word found in the text after the 
/// given start, with the given bounding characters.
/// </summary>
const char* TextBlock::FindWordEnd(const char* pStart, const string_view& breakFilter) const
{
    return FindEnd(pStart, breakFilter, '!', '~');
}

/// <summary>
/// Returns pointer to first character in a word found in the text before the 
/// given start, with the given bounding characters.
/// </summary>
const char* TextBlock::FindLastWord(const char* pStart, const string_view& breakFilter) const
{
    return FindLastStart(pStart, breakFilter, '!', '~');
}

TextBlock TextBlock::GetWord(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = FindStart(pStart, breakFilter, min, max);
    const char* pEnd = FindEnd(pStart, breakFilter, min, max);
    return TextBlock(pStart, pEnd);
}

TextBlock TextBlock::GetLastWord(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = FindLastStart(pStart, breakFilter, min, max);
    const char* pEnd = FindEnd(pStart, breakFilter, min, max);
    return TextBlock(pStart, pEnd);
}

const char* TextBlock::Find(const char* substr, size_t subLen, const char* pStart)
{
    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(&GetBack(), pStart);

    if (substr[subLen - 1] == '\0')
        subLen--;

    if (pStart >= GetData() && remLen >= subLen)
    {
        for (size_t i = 0; i < remLen; i++)
        {
            if (pStart[i] == substr[0])
            {
                size_t matchLen = 1;

                for (size_t j = 1; j < subLen; j++)
                {
                    if (pStart[i + j] != substr[j])
                        break;

                    matchLen++;
                }

                if (matchLen == subLen)
                    return &pStart[i];
            }
        }
    }

    return nullptr;
}

/// <summary>
/// Returns pointer to first character in a word found in the text after the given start
/// </summary>
const char* TextBlock::FindStart(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = ClampPtr(pStart, GetData(), &GetBack());

    while (pStart < &GetBack() && !(*pStart >= min && *pStart <= max) && breakFilter.find(*pStart) == string::npos)
        pStart++;

    return pStart;
}

/// <summary>
/// Returns the position of the last character in the word starting at the given pointer
/// </summary>
const char* TextBlock::FindEnd(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = ClampPtr(pStart, GetData(), &GetBack());
    const char* pCh = pStart;

    while (pCh <= &GetBack() && GetIsRangeChar(*pCh, breakFilter, min, max))
    {
        pStart = pCh;
        pCh++;
    }

    return pStart;
}

/// <summary>
/// Returns pointer to first character in a word found in the text before the given start
/// </summary>
const char* TextBlock::FindLastStart(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = ClampPtr(pStart, GetData(), &GetBack());
    const char* pCh = pStart;

    while (pCh > GetData() && !(*pCh >= min && *pCh <= max) && breakFilter.find(*pCh) == string::npos)
        pCh--;

    do
    {
        pStart = pCh;
        pCh--;
    } 
    while (pCh >= GetData() && GetIsRangeChar(*pCh, breakFilter, min, max));

    return pStart;
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const char ch, const char* pStart)
{
    return Find(&ch, 1, pStart);
}

/// <summary>
/// Writes a null-terminated copy of the text span's contents to the given buffer.
/// </summary>
void TextBlock::CopyTo(char* dst, size_t maxLength) const
{
    size_t len = std::min(length, maxLength),
        end = std::min(len - 1, maxLength - 1);

    memcpy(dst, pStart, len);
    dst[end] = '\0';
}

/// <summary>
/// Returns a copy of the range as a new string
/// </summary>
string TextBlock::ToString()
{
    return string(pStart, length);
}

/// <summary>
/// Returns a copy of the range as a new string
/// </summary>
string TextBlock::ToString(const char* pStart, const char* pEnd)
{
    size_t len = UnsignedDelta(pEnd, pStart);
    return string(pStart, len);
}

/// <summary>
/// Returns true if the given character falls inside the given range and isn't in the given string filter.
/// </summary>
bool TextBlock::GetIsRangeChar(char ch, const string_view& filter, char min, char max)
{
    return ch >= min && ch <= max && filter.find(ch) == string::npos;
}