#include "../src/utils/include/TextBlock.hpp"

using namespace Replica;

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

TextBlock::TextBlock(const char* pStart) : Span(pStart, 1)
{ }

TextBlock::TextBlock(const char* pStart, size_t length) : Span(pStart, length)
{ }

TextBlock::TextBlock(const char* pStart, const char* pEnd) : Span(pStart, pEnd)
{ }

TextBlock::TextBlock(string& str) : Span(&str[0], str.length())
{ }

TextBlock::TextBlock(string_view& str) : Span(&str[0], str.length())
{ }

TextBlock::TextBlock(Span<char>& span) : Span(span.GetFirst(), span.GetLength())
{ }

/// <summary>
/// Returns the total number of the given character in the text block, starting
/// from the given point.
/// </summary>
int TextBlock::FindCount(const char ch, const char* pStart) const
{
    int count = 0;

    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(GetLast(), pStart);

    if (pStart >= GetFirst() && remLen >= 1)
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
const char* TextBlock::Find(const string_view& substr, const char* pStart)
{
    return Find(&substr[0], (int)substr.length(), pStart);
}

/// <summary>
/// Finds position of the first character of the first matching occurence of 
/// the given substring, starting from the given pointer. Doesn't stop on '\0'.
/// </summary>
const char* TextBlock::Find(const char* substr, int subLen, const char* pStart) const
{
    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(GetLast(), pStart);

    if (substr[subLen - 1] == '\0')
        subLen--;

    if (pStart >= GetFirst() && remLen >= subLen)
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
                    return (char*)&pStart[i];
            }
        }
    }

    return nullptr;
}

const char* TextBlock::Find(const char* substr, int subLen, const char* pStart)
{
    if (pStart == nullptr)
        pStart = this->pStart;

    size_t remLen = UnsignedDelta(GetLast(), pStart);

    if (substr[subLen - 1] == '\0')
        subLen--;

    if (pStart >= GetFirst() && remLen >= subLen)
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
                    return (char*)&pStart[i];
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
    pStart = ClampPtr(pStart, GetFirst(), GetLast());

    while (pStart < GetLast() && !(*pStart >= min && *pStart <= max) && breakFilter.find(*pStart) == string::npos)
        pStart++;

    return (char*)pStart;
}

/// <summary>
/// Returns the position of the last character in the word starting at the given pointer
/// </summary>
const char* TextBlock::FindEnd(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = ClampPtr(pStart, GetFirst(), GetLast());
    const char* pCh = pStart;

    while (pCh <= GetLast() && GetIsRangeChar(*pCh, breakFilter, min, max))
    {
        pStart = pCh;
        pCh++;
    }

    return (char*)pStart;
}

/// <summary>
/// Returns pointer to first character in a word found in the text before the given start
/// </summary>
const char* TextBlock::FindLastStart(const char* pStart, const string_view& breakFilter, char min, char max) const
{
    pStart = ClampPtr(pStart, GetFirst(), GetLast());
    const char* pCh = pStart;

    while (pCh > GetFirst() && !(*pCh >= min && *pCh <= max) && breakFilter.find(*pCh) == string::npos)
        pCh--;

    do
    {
        pStart = pCh;
        pCh--;
    } 
    while (pCh >= GetFirst() && GetIsRangeChar(*pCh, breakFilter, min, max));

    return (char*)pStart;
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
/// Returns true if the given character falls inside the given range and isn't in the given string filter.
/// </summary>
bool TextBlock::GetIsRangeChar(char ch, const string_view& filter, char min, char max)
{
    return ch >= min && ch <= max && filter.find(ch) == string::npos;
}