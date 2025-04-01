#include "pch.hpp"
#include "ReplicaEffects/ShaderDataHandles.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderRegistryMap.hpp"
#include "ReplicaEffects/ShaderLibGen/ShaderRegistryBuilder.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderRegistryMap::ShaderRegistryMap(ShaderRegistryDef&& def) :
	stringMap(std::move(def.stringIDs)),
	constants(std::move(def.constants)),
	cbufDefs(std::move(def.cbufDefs)),
	ioElements(std::move(def.ioElements)),
	resources(std::move(def.resources)),
	cbufGroups(std::move(def.cbufGroups)),
	ioSignatures(std::move(def.ioSignatures)),
	resGroups(std::move(def.resGroups)),
	shaders(std::move(def.shaders)),
	effects(std::move(def.effects))
{ }

ShaderRegistryMap::ShaderRegistryMap(const ShaderRegistryDef& def) :
	stringMap(def.stringIDs),
	constants(def.constants),
	cbufDefs(def.cbufDefs),
	ioElements(def.ioElements),
	resources(def.resources),
	cbufGroups(def.cbufGroups),
	ioSignatures(def.ioSignatures),
	resGroups(def.resGroups),
	shaders(def.shaders),
	effects(def.effects)
{ }

const StringIDMap& ShaderRegistryMap::GetStringMap() const { return stringMap; }

bool ShaderRegistryMap::TryGetStringID(string_view str, uint& id) const
{
	return stringMap.TryGetStringID(str, id);
}

string_view ShaderRegistryMap::GetString(uint id) const { return stringMap.GetString(id); }

uint ShaderRegistryMap::GetStringCount() const { return stringMap.GetStringCount(); }

const EffectDef& ShaderRegistryMap::GetEffect(uint effectID) const { return effects[ShaderRegistryBuilder::GetIndex(effectID)]; }

const ShaderDef& ShaderRegistryMap::GetShader(uint shaderID) const { return shaders[ShaderRegistryBuilder::GetIndex(shaderID)]; }

const IDynamicArray<uint>& ShaderRegistryMap::GetResourceGroup(uint groupID) const { return resGroups[ShaderRegistryBuilder::GetIndex(groupID)]; }

const IDynamicArray<uint>& ShaderRegistryMap::GetIOLayout(uint layoutID) const { return ioSignatures[ShaderRegistryBuilder::GetIndex(layoutID)]; }

const IDynamicArray<uint>& ShaderRegistryMap::GetCBufGroup(uint groupID) const { return cbufGroups[ShaderRegistryBuilder::GetIndex(groupID)]; }

const ResourceDef& ShaderRegistryMap::GetResource(const uint resID) const { return resources[ShaderRegistryBuilder::GetIndex(resID)]; }

const IOElementDef& ShaderRegistryMap::GetIOElement(const uint id) const { return ioElements[ShaderRegistryBuilder::GetIndex(id)]; }

const ConstBufDef& ShaderRegistryMap::GetConstBuf(const uint cbufID) const { return cbufDefs[ShaderRegistryBuilder::GetIndex(cbufID)]; }

const ConstDef& ShaderRegistryMap::GetConstant(const uint constID) const { return constants[ShaderRegistryBuilder::GetIndex(constID)]; }
