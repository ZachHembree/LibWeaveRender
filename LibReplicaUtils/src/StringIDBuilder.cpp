#include "pch.hpp"
#include "ReplicaUtils/StringIDBuilder.hpp"

using namespace Replica;

StringIDBuilder::StringIDBuilder() = default;

StringIDBuilder::StringIDBuilder(StringIDBuilder&& other) noexcept = default;

StringIDBuilder& StringIDBuilder::operator=(StringIDBuilder&& other) noexcept = default;

/// <summary>
/// Returns the ID corresponding to the given string. Adds a copy of
/// the string to the map if it hasn't been added previously.
/// </summary>
uint StringIDBuilder::GetOrAddStringID(std::string_view str)
{
    StringSpan ss = GetTmpStringSpan(str, stringData);
    auto it = idMap.find(ss);

    if (it != idMap.end())
    {
        RemoveTmpStringSpan(ss, stringData);
        return it->second;
    }
    else
    {
        const uint id = static_cast<uint>(strings.GetLength());
        strings.emplace_back(ss);
        idMap.emplace(ss, id);

        return id;
    }
}

/// <summary>
/// Returns true if the string exists in the map and retrieves its ID
/// </summary>
bool StringIDBuilder::TryGetStringID(std::string_view str, uint& id) const
{
    StringSpan ss = GetTmpStringSpan(str, textBuf);
    auto it = idMap.find(ss);
    RemoveTmpStringSpan(ss, textBuf);

    if (it != idMap.end())
    {
        id = it->second;
        return true;
    }

    id = INVALID_ID;
    return false;
}

/// <summary>
/// Returns the string corresponding to the given ID
/// </summary>
std::string_view StringIDBuilder::GetString(uint id) const { return strings[id]; }

/// <summary>
/// Returns the total number of strings mapped
/// </summary>
/// <returns></returns>
uint StringIDBuilder::GetStringCount() const { return static_cast<uint>(strings.GetLength()); }

void StringIDBuilder::WriteDefinition(StringIDMapDef& def) const
{
    const uint count = GetStringCount();
    uint charCount = 0;

    // Calculate concatenated string buffer size and substring layout
    def.substrings.reserve(2 * count);

    for (uint i = 0; i < count; i++)
    {
        const uint subLen = (uint)strings[i].GetLength();
        def.substrings.push_back(charCount);
        def.substrings.push_back(subLen - 1); // Exclude null terminator
        charCount += subLen;
    }

    // Copy concatenated string buffer
    def.stringData.append(stringData);
}

void StringIDBuilder::Clear()
{
    strings.clear();
    idMap.clear();
    textBuf.clear();
    stringData.clear();
}

/// <summary>
/// Copies a temporary string into the text buffer, but not the lookup tables
/// </summary>
StringSpan StringIDBuilder::GetTmpStringSpan(string_view str, string& buf)
{
    const size_t bufStart = buf.length();
    size_t len = str.length();
    buf.append(str);

    if (str.back() != '\0')
    {
        buf.push_back('\0');
        len++;
    }

    return StringSpan(buf, bufStart, len);
}

/// <summary>
/// Erases a temporary string from the end of the text buffer
/// </summary>
void StringIDBuilder::RemoveTmpStringSpan(const StringSpan& ss, string& buf)
{
    buf.erase(buf.length() - ss.GetLength());
}
