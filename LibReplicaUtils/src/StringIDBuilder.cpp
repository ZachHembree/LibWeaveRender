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
        const uint id = static_cast<uint>(substrings.GetLength() / 2);
        substrings.Add((uint)ss.GetIndex());
        substrings.Add((uint)ss.GetLength() - 1); // Exclude null-terminator
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
string_view StringIDBuilder::GetString(uint id) const 
{ 
    const uint start = substrings[id * 2],
        length = substrings[id * 2 + 1];

    return string_view(&stringData[start], length);
}

/// <summary>
/// Returns the total number of strings mapped
/// </summary>
uint StringIDBuilder::GetStringCount() const { return (uint)(substrings.GetLength() / 2); }

StringIDMapDef::Handle StringIDBuilder::GetDefinition() const
{
    return 
    {
        .pSubstrings = &substrings,
        .pStringData = &stringData
    };
}

void StringIDBuilder::Clear()
{
    substrings.Clear();
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
