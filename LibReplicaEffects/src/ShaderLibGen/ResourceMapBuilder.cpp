#include "pch.hpp"
#include "ShaderLibGen/ResourceMapBuilder.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderRegistryBuilder::ShaderRegistryBuilder() :
	resCount(0),
	uniqueResCount(0)
{ }

void ShaderRegistryBuilder::Clear()
{
	stringIDs.Clear();
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

uint ShaderRegistryBuilder::GetOrAddStringID(string&& str) { return stringIDs.GetOrAddStringID(std::move(str)); }

uint ShaderRegistryBuilder::GetOrAddStringID(string_view str) { return stringIDs.GetOrAddStringID(str); }

// Add helpers
uint ShaderRegistryBuilder::GetOrAddConstant(const ConstDef& constDef) { return GetOrAddValue(ConstDef(constDef), constants); }

uint ShaderRegistryBuilder::GetOrAddConstantBuffer(ConstBufDef&& cbufDef) { return GetOrAddValue(std::move(cbufDef), cbufDefs); }

uint ShaderRegistryBuilder::GetOrAddIOElement(const IOElementDef& ioDef) { return GetOrAddValue(IOElementDef(ioDef), ioElements); }

uint ShaderRegistryBuilder::GetOrAddResource(const ResourceDef& resDef) { return GetOrAddValue(ResourceDef(resDef), resources); }

uint ShaderRegistryBuilder::GetOrAddCBufGroup(DynamicArray<uint>&& bufGroup) { return GetOrAddValue(std::move(bufGroup), cbufGroups); }

uint ShaderRegistryBuilder::GetOrAddIOLayout(DynamicArray<uint>&& signature) { return GetOrAddValue(std::move(signature), ioSignatures); }

uint ShaderRegistryBuilder::GetOrAddResGroup(DynamicArray<uint>&& resGroup) { return GetOrAddValue(std::move(resGroup), resGroups); }

uint ShaderRegistryBuilder::GetOrAddShader(ShaderDef&& shader) { return GetOrAddValue(std::move(shader), shaders); }

uint ShaderRegistryBuilder::GetOrAddEffect(EffectDef&& effect) { return GetOrAddValue(std::move(effect), effects); }

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
	def.cbufGroups = DynamicArray(cbufGroups);
	def.ioSignatures = DynamicArray(ioSignatures);
	def.resGroups = DynamicArray(resGroups);
	def.shaders = DynamicArray(shaders);
	def.effects = DynamicArray(effects);

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
