#include "pch.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderRegistryBuilder::ShaderRegistryBuilder() :
	resCount(0),
	uniqueResCount(0)
{ }

void ShaderRegistryBuilder::Clear()
{
	stringIDs.Clear();
	idBuffer.clear();
	resourceSet.clear();
	constants.clear();
	cbufDefs.clear();
	ioElements.clear();
	resources.clear();
	cbufGroups.clear();
	ioSignatures.clear();
	resGroups.clear();
	shaders.clear();
	effects.clear();
	resCount = 0;

	uniqueResCount = 0;
}

int ShaderRegistryBuilder::GetResourceCount() const { return resCount; }

int ShaderRegistryBuilder::GetUniqueResCount() const { return uniqueResCount; }

uint ShaderRegistryBuilder::GetOrAddStringID(string_view str) { return stringIDs.GetOrAddStringID(str); }

// Add helpers
uint ShaderRegistryBuilder::GetOrAddConstant(const ConstDef& constDef) { return GetOrAddValue(ConstDef(constDef), constants); }

uint ShaderRegistryBuilder::GetOrAddConstantBuffer(const ConstBufDef& cbufDef) { return GetOrAddValue(cbufDef, cbufDefs); }

uint ShaderRegistryBuilder::GetOrAddIOElement(const IOElementDef& ioDef) { return GetOrAddValue(IOElementDef(ioDef), ioElements); }

uint ShaderRegistryBuilder::GetOrAddResource(const ResourceDef& resDef) { return GetOrAddValue(ResourceDef(resDef), resources); }

uint ShaderRegistryBuilder::GetOrAddCBufGroup(const IDynamicArray<uint>& bufGroup) { return GetOrAddValue(bufGroup, cbufGroups); }

uint ShaderRegistryBuilder::GetOrAddIOLayout(const IDynamicArray<uint>& signature) { return GetOrAddValue(signature, ioSignatures); }

uint ShaderRegistryBuilder::GetOrAddResGroup(const IDynamicArray<uint>& resGroup) { return GetOrAddValue(resGroup, resGroups); }

uint ShaderRegistryBuilder::GetOrAddShader(const ShaderDef& shader) { return GetOrAddValue(shader, shaders); }

uint ShaderRegistryBuilder::GetOrAddEffect(const EffectDef& effect) { return GetOrAddValue(effect, effects); }

// Getter helpers
const ConstDef& ShaderRegistryBuilder::GetConstant(const uint id) const { return constants.GetValue(id); }

const ConstBufDef& ShaderRegistryBuilder::GetConstantBuffer(const uint id) const { return cbufDefs.GetValue(id); }

const IOElementDef& ShaderRegistryBuilder::GetIOElement(const uint id) const { return ioElements.GetValue(id); }

const ResourceDef& ShaderRegistryBuilder::GetResource(const uint id) const { return resources.GetValue(id); }

const IDynamicArray<uint>& ShaderRegistryBuilder::GetCBufGroup(const uint id) const { return cbufGroups.GetValue(id); }

const IDynamicArray<uint>& ShaderRegistryBuilder::GetIOLayout(const uint id) const { return ioSignatures.GetValue(id); }

const IDynamicArray<uint>& ShaderRegistryBuilder::GetResGroup(const uint id) const { return resGroups.GetValue(id); }

const ShaderDef& ShaderRegistryBuilder::GetShader(const uint id) const { return shaders.GetValue(id); }

const EffectDef& ShaderRegistryBuilder::GetEffect(const uint id) const { return effects.GetValue(id); }

ShaderRegistryDef ShaderRegistryBuilder::ExportDefinition() const
{
	ShaderRegistryDef def;
	def.stringIDs = stringIDs.ExportDefinition();
	def.constants = DynamicArray(constants);
	def.cbufDefs = DynamicArray(cbufDefs);
	def.ioElements = DynamicArray(ioElements);
	def.resources = DynamicArray(resources);
	def.shaders = DynamicArray(shaders);
	def.effects = DynamicArray(effects);

	def.cbufGroups = DynamicArray<DynamicArray<uint>>(cbufGroups.GetLength());

	for (uint i = 0; i < cbufGroups.GetLength(); i++)
		def.cbufGroups[i] = DynamicArray<uint>(cbufGroups[i]);

	def.ioSignatures = DynamicArray<DynamicArray<uint>>(ioSignatures.GetLength());

	for (uint i = 0; i < ioSignatures.GetLength(); i++)
		def.ioSignatures[i] = DynamicArray<uint>(ioSignatures[i]);

	def.resGroups = DynamicArray<DynamicArray<uint>>(resGroups.GetLength());

	for (uint i = 0; i < resGroups.GetLength(); i++)
		def.resGroups[i] = DynamicArray<uint>(resGroups[i]);

	return def;
}

uint ShaderRegistryBuilder::SetResourceType(uint index, ResourceType type) { return ((index & 0x00FFFFFFu) | ((uint)type << 24u)); }

// ID/Index utils
ResourceType ShaderRegistryBuilder::GetResourceType(uint id) { return (ResourceType)(id >> 24u); }

uint ShaderRegistryBuilder::GetIndex(uint id) { return (id & 0x00FFFFFFu); }

// Heterogeneous hashing handle
HashHandle::HashHandle() : id(-1), pGetHash(nullptr), pGetIsEqual(nullptr)
{ }

HashHandle::HashHandle(const uint id, const HashFunc& getHash, const IsEqFunc& getIsEq) :
	id(id), pGetHash(&getHash), pGetIsEqual(&getIsEq)
{ }

uint HashHandle::GetID() const { return id; }

uint HashHandle::GetIndex() const { return ShaderRegistryBuilder::GetIndex(id); }

ResourceType HashHandle::GetType() const { return ShaderRegistryBuilder::GetResourceType(id); }

size_t HashHandle::GetHash() const { return (*pGetHash)(*this); }

bool HashHandle::operator==(const HashHandle& rhs) const
{
	if (this == &rhs)
		return true;
	else if (GetType() == rhs.GetType())
		return (*pGetIsEqual)(*this, rhs);
	else
		return false;
}
