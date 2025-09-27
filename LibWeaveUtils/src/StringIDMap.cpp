#include "pch.hpp"
#include "WeaveUtils/StringIDMap.hpp"

using namespace Weave;

void StringIDMapDef::Clear()
{
    substrings.Clear();
    stringData.clear();
}

StringIDMap::StringIDMap(const StringIDMapDef::Handle& def) :
    pDef(new StringIDMapDef(def.GetCopy()))
{
    InitMapData();
}

StringIDMap::StringIDMap(StringIDMapDef&& def) :
    pDef(new StringIDMapDef(std::move(def)))
{
    InitMapData();
}

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

std::string_view StringIDMap::GetString(uint id) const 
{ 
    size_t start = pDef->substrings[id * 2];
    size_t length = pDef->substrings[id * 2 + 1];
    return string_view(&pDef->stringData[start], length); 
}

uint StringIDMap::GetStringCount() const { return (uint)(pDef->substrings.GetLength() / 2); }

StringIDMapDef::Handle StringIDMap::GetDefinition() const { return pDef->GetHandle(); }

void StringIDMap::InitMapData()
{
    const uint strCount = (uint)(pDef->substrings.GetLength() / 2);
    idMap.reserve(strCount);

    for (uint i = 0; i < strCount; i++)
    {
        idMap.emplace(GetString(i), i);
    }
}
