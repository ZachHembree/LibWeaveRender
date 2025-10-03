#include "pch.hpp"
#include "WeaveUtils/StringIDMap.hpp"
#include "WeaveUtils/StringIDBuilder.hpp"

using namespace Weave;

// Map def

void StringIDMapDef::Clear()
{
    substrings.Clear();
    stringData.clear();
}

// StringIDMap

StringIDMap::StringIDMap(const StringIDMapDef::Handle& def) :
    pDef(new StringIDMapDef(def.GetCopy()))
{ }

StringIDMap::StringIDMap(StringIDMapDef&& def) :
    pDef(new StringIDMapDef(std::move(def)))
{ }

bool StringIDMap::TryGetStringID(std::string_view str, uint& id) const
{
    // Reverse lookup map init is deferred
    if (idMap.empty() && !pDef->substrings.IsEmpty())
        InitMapData();

    auto it = idMap.find(str);

    if (it != idMap.end())
    {
        id = it->second;
        return true;
    }

    id = g_InvalidID32;
    return false;
}

const StringSpan StringIDMap::GetString(uint id) const
{ 
    WV_CHECK_MSG(id != g_InvalidID32, "String ID is invalid");

    size_t start = pDef->substrings[id * 2];
    size_t length = pDef->substrings[id * 2 + 1];
    return StringSpan(const_cast<string&>(pDef->stringData), start, length);
}

uint StringIDMap::GetStringCount() const { return (uint)(pDef->substrings.GetLength() / 2); }

StringIDMapDef::Handle StringIDMap::GetDefinition() const { return pDef->GetHandle(); }

void StringIDMap::InitMapData() const
{
    const uint strCount = (uint)(pDef->substrings.GetLength() / 2);
    idMap.reserve(strCount);

    for (uint i = 0; i < strCount; i++)   
        idMap.emplace(GetString(i), i);
}

// Alias map

StringIDMapAlias::StringIDMapAlias(const StringIDMapDef::Handle& def, StringIDBuilder& parent) :
    pParent(&parent),
    idAliases(def.pSubstrings->GetLength() / 2)
{
    parent.GetOrAddStrings(*def.pSubstrings, *def.pStringData, idAliases);
}

const StringIDBuilder& StringIDMapAlias::GetParent() const  { return *pParent; }

uint StringIDMapAlias::GetAliasedID(uint id) const { return idAliases[id]; }

uint StringIDMapAlias::GetStringCount() const
{
    return (uint)idAliases.GetLength();
}

bool StringIDMapAlias::TryGetStringID(std::string_view str, uint& id) const
{
    return pParent->TryGetStringID(str, id);
}

const StringSpan StringIDMapAlias::GetString(uint id) const
{
    return pParent->GetString(id);
}

StringIDMapDef::Handle StringIDMapAlias::GetDefinition() const
{
    return pParent->GetDefinition();
}
