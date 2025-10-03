#include "pch.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderRegistryMap::ShaderRegistryMap(const ShaderRegistryDef::Handle& def, const StringIDMapDef::Handle& strDef) :
	pRegDef(new ShaderRegistryDef(def.GetCopy())),
	pStringIDs(new StringIDMap(strDef))
{ }

ShaderRegistryMap::ShaderRegistryMap(ShaderRegistryDef&& def, StringIDMapDef&& strDef) :
	pRegDef(new ShaderRegistryDef(std::move(def))),
	pStringIDs(new StringIDMap(std::move(strDef)))
{ }

ShaderRegistryMap::ShaderRegistryMap(const ShaderRegistryDef::Handle& def, const StringIDMapDef::Handle& strDef, 
	StringIDBuilder& stringIDs) :
	pRegDef(new ShaderRegistryDef(def.GetCopy())),
	pStringIDs(new StringIDMapAlias(strDef, stringIDs))
{ 
	InitStringIDAliases();
}

ShaderRegistryMap::ShaderRegistryMap(ShaderRegistryDef&& def, const StringIDMapDef::Handle& strDef, 
	StringIDBuilder& stringIDs) :
	pRegDef(new ShaderRegistryDef(std::move(def))),
	pStringIDs(new StringIDMapAlias(strDef, stringIDs))
{
	InitStringIDAliases();
}

void ShaderRegistryMap::InitStringIDAliases()
{
	for (EffectDef& def : pRegDef->effects)
		def.nameID = GetStringMap().GetAliasedID(def.nameID);

	for (ShaderDef& def : pRegDef->shaders)
	{
		def.fileStringID = GetStringMap().GetAliasedID(def.fileStringID);
		def.nameID = GetStringMap().GetAliasedID(def.nameID);
	}

	for (ResourceDef& def : pRegDef->resources)
		def.stringID = GetStringMap().GetAliasedID(def.stringID);

	for (IOElementDef& def : pRegDef->ioElements)
		def.semanticID = GetStringMap().GetAliasedID(def.semanticID);

	for (ConstBufDef& def : pRegDef->cbufDefs)
		def.stringID = GetStringMap().GetAliasedID(def.stringID);

	for (ConstDef& def : pRegDef->constants)
		def.stringID = GetStringMap().GetAliasedID(def.stringID);
}

ShaderRegistryMap::~ShaderRegistryMap() = default;

const IStringIDMap& ShaderRegistryMap::GetStringMap() const { return *pStringIDs; }

bool ShaderRegistryMap::TryGetStringID(string_view str, uint& id) const
{
	return pStringIDs->TryGetStringID(str, id);
}

StringSpan ShaderRegistryMap::GetString(uint id) const { return pStringIDs->GetString(id); }

uint ShaderRegistryMap::GetStringCount() const { return pStringIDs->GetStringCount(); }

const EffectDef& ShaderRegistryMap::GetEffect(uint effectID) const 
{ return pRegDef->effects[ShaderRegistryBuilder::GetIndex(effectID)]; }

const ShaderDef& ShaderRegistryMap::GetShader(uint shaderID) const 
{ return pRegDef->shaders[ShaderRegistryBuilder::GetIndex(shaderID)]; }

ByteSpan ShaderRegistryMap::GetByteCode(uint byteCodeID) const
{ return pRegDef->binSpans[ShaderRegistryBuilder::GetIndex(byteCodeID)]; }

IDSpan ShaderRegistryMap::GetIDGroup(uint groupID) const
{ return pRegDef->idGroups[ShaderRegistryBuilder::GetIndex(groupID)]; }

const ResourceDef& ShaderRegistryMap::GetResource(const uint resID) const 
{ return pRegDef->resources[ShaderRegistryBuilder::GetIndex(resID)]; }

const IOElementDef& ShaderRegistryMap::GetIOElement(const uint id) const 
{ return pRegDef->ioElements[ShaderRegistryBuilder::GetIndex(id)]; }

const ConstBufDef& ShaderRegistryMap::GetConstBuf(const uint cbufID) const 
{ return pRegDef->cbufDefs[ShaderRegistryBuilder::GetIndex(cbufID)]; }

const ConstDef& ShaderRegistryMap::GetConstant(const uint constID) const 
{ return pRegDef->constants[ShaderRegistryBuilder::GetIndex(constID)]; }

ShaderRegistryDef::Handle ShaderRegistryMap::GetDefinition() const { return pRegDef->GetHandle(); }