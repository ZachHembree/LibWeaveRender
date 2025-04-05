#include "pch.hpp"
#include "ReplicaUtils/StringIDBuilder.hpp"

using namespace Replica;

StringIDBuilder::StringIDBuilder() = default;

StringIDBuilder::StringIDBuilder(StringIDBuilder&& other) noexcept = default;

StringIDBuilder& StringIDBuilder::operator=(StringIDBuilder&& other) noexcept = default;

/// <summary>
/// Returns the ID corresponding to the given string. Moves
/// the string to the map if it hasn't been added previously.
/// </summary>
uint StringIDBuilder::GetOrAddStringID(std::string&& str) noexcept
{
    if (str.back() != '\0')
        str.push_back('\0');

    auto it = idMap.find(str);

    if (it != idMap.end())
    {
        return it->second;
    }
    else
    {
        const uint id = static_cast<uint>(strings.size());
        const std::string& storedStr = strings.emplace_back(std::move(str));
        idMap.emplace(std::string_view(storedStr), id);

        return id;
    }
}

/// <summary>
/// Returns the ID corresponding to the given string. Adds a copy of
/// the string to the map if it hasn't been added previously.
/// </summary>
uint StringIDBuilder::GetOrAddStringID(std::string_view str)
{
    if (str.back() != '\0')
    {
        textBuf.clear();
        textBuf.append(str);
        textBuf.push_back('\0');
        str = textBuf;
    }

    auto it = idMap.find(str);

    if (it != idMap.end())
    {
        return it->second;
    }
    else
    {
        const uint id = static_cast<uint>(strings.size());
        const std::string& storedStr = strings.emplace_back(string(str));
        idMap.emplace(std::string_view(storedStr), id);

        return id;
    }
}

/// <summary>
/// Returns true if the string exists in the map and retrieves its ID
/// </summary>
bool StringIDBuilder::TryGetStringID(std::string_view str, uint& id) const
{
    auto it = idMap.find(str);

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
uint StringIDBuilder::GetStringCount() const { return static_cast<uint>(strings.size()); }

/// <summary>
/// Returns a copy of the ID generator's current state in a compact, serializable format
/// </summary>
StringIDMapDef StringIDBuilder::ExportDefinition() const
{
    const uint count = GetStringCount();
    uint charCount = 0;

    // Calculate concatenated string buffer size and substring layout
    StringIDMapDef def = {};
    def.substrings = DynamicArray<uint>(2 * count);

    for (uint i = 0; i < count; i++)
    {
        const uint subLen = (uint)strings[i].size();
        def.substrings[2 * i] = charCount;
        def.substrings[2 * i + 1] = subLen - 1; // Exclude null terminator
        charCount += subLen;
    }

    // Copy strings into concatenated buffer
    def.stringData = DynamicArray<char>(charCount);
    uint charStart = 0;

    for (uint i = 0; i < count; i++)
    {
        const uint remLen = (uint)def.stringData.GetLength() - charStart;
        const string& substring = strings[i];
        memcpy_s(&def.stringData[charStart], remLen, substring.data(), substring.size());
        charStart += static_cast<uint>(substring.size());
    }

    return def;
}

void StringIDBuilder::Clear()
{
    strings.clear();
    idMap.clear();
}
