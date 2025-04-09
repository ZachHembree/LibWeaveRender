#include "pch.hpp"
#include "ReplicaUtils/StringIDMap.hpp"

using namespace Replica;

StringIDMapDef::StringIDMapDef() = default;

StringIDMapDef::StringIDMapDef(StringIDMapDef&& other) noexcept = default;

StringIDMapDef& StringIDMapDef::operator=(StringIDMapDef&& other) noexcept = default;

void StringIDMapDef::Clear()
{
    substrings.clear();
    stringData.clear();
}

StringIDMap::StringIDMap(StringIDMapDef&& def) noexcept :
    stringBuf(std::move(def.stringData))
{
    InitMapData(def);
}

StringIDMap::StringIDMap(const StringIDMapDef& def) :
    stringBuf(def.stringData)
{
    InitMapData(def);
}

/// <summary>
/// Returns true if the string exists in the map and retrieves its ID
/// </summary>
bool StringIDMap::TryGetStringID(std::string_view str, uint& id) const
{
    auto it = idMap.find(str);

    if (it != idMap.end())
    {
        id = it->second;
        return true;
    }

    id = StringIDMap::INVALID_ID;
    return false;
}

/// <summary>
/// Returns the string_view corresponding to the given ID
/// </summary>
std::string_view StringIDMap::GetString(uint id) const { return substrings[id]; }

/// <summary>
/// Returns the number of unique strings in the map
/// </summary>
uint StringIDMap::GetStringCount() const { return static_cast<uint>(substrings.GetLength()); }

void StringIDMap::InitMapData(const StringIDMapDef& def)
{
    substrings = UniqueArray<string_view>(def.substrings.GetLength() / 2);
    idMap.reserve(substrings.GetLength());

    for (size_t i = 0; i < def.substrings.GetLength(); i += 2)
    {
        const uint startIndex = def.substrings[i];
        const uint subLen = def.substrings[i + 1];
        const char* pStart = &stringBuf[startIndex];
        substrings[i / 2] = string_view(pStart, subLen);
    }

    for (size_t i = 0; i < substrings.GetLength(); i++)
    {
        idMap.emplace(substrings[i], static_cast<uint>(i));
    }
}
