#include "pch.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderRegistryMap::ShaderRegistryMap(const StringIDMapDef::Handle& strDef, const ShaderRegistryDef::Handle& def) :
	pRegDef(new ShaderRegistryDef(def.GetCopy())),
	stringMap(strDef)
{ }

ShaderRegistryMap::ShaderRegistryMap(StringIDMapDef&& strDef, ShaderRegistryDef&& def) :
	pRegDef(new ShaderRegistryDef(std::move(def))),
	stringMap(std::move(strDef))
{ }

const StringIDMap& ShaderRegistryMap::GetStringMap() const { return stringMap; }

bool ShaderRegistryMap::TryGetStringID(string_view str, uint& id) const
{
	return stringMap.TryGetStringID(str, id);
}

string_view ShaderRegistryMap::GetString(uint id) const { return stringMap.GetString(id); }

uint ShaderRegistryMap::GetStringCount() const { return stringMap.GetStringCount(); }

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
