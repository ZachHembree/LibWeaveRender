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

std::string_view StringIDMap::GetString(uint id) const 
{ 
    WV_CHECK_MSG(id != g_InvalidID32, "String ID is invalid");

    size_t start = pDef->substrings[id * 2];
    size_t length = pDef->substrings[id * 2 + 1];
    return string_view(&pDef->stringData[start], length); 
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
    const Vector<uint>& substrings = *def.pSubstrings;
    const string& stringData = *def.pStringData;

    for (uint id = 0; id < idAliases.GetLength(); id++)
    {
        size_t start = substrings[id * 2];
        size_t length = substrings[id * 2 + 1];
        string_view str(&stringData[start], length);
        idAliases[id] = parent.GetOrAddStringID(str) | g_Bit32; // Set bit 32 to indicate aliased ID
    }
}

const StringIDBuilder& StringIDMapAlias::GetParent() const  { return *pParent; }

uint StringIDMapAlias::GetParentStringID(uint localID) const { return idAliases[localID]; }

bool StringIDMapAlias::TryGetStringID(std::string_view str, uint& id) const
{
    const bool success = pParent->TryGetStringID(str, id);
    id |= g_Bit32; // Indicate parent owned aliased ID

    return success;
}

std::string_view StringIDMapAlias::GetString(uint id) const
{
    WV_CHECK_MSG(id != g_InvalidID32, "String ID is invalid");

    // IDs with bit 32 set indicate an aliased ID owned by a parent builder
    // Otherwise the ID is local to the current map
    if ((id & g_Bit32) > 0)
        return pParent->GetString(id);
    else
        return pParent->GetString(idAliases[id]);
}

uint StringIDMapAlias::GetStringCount() const
{
    return (uint)idAliases.GetLength();
}

StringIDMapDef::Handle StringIDMapAlias::GetDefinition() const
{
    return pParent->GetDefinition();
}
