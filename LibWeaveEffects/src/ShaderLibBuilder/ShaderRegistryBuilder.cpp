#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"

using namespace Weave;
using namespace Weave::Effects;

ShaderRegistryBuilder::ShaderRegistryBuilder() :
	resCount(0),
	uniqueResCount(0)
{
}

void ShaderRegistryBuilder::Clear()
{
	FX_ASSERT_MSG((idBufPool.GetObjectsOutstanding() + byteCodePool.GetObjectsOutstanding()) == 0,
		"Temporary buffers must be returned before finalizing or exporting shader regsitry.");

	stringIDs.Clear();
	resourceSet.clear();
	idBuffer.Clear();
	byteBuffer.Clear();

	constants.Clear();
	cbufDefs.Clear();
	ioElements.Clear();
	resources.Clear();

	shaders.Clear();
	effects.Clear();

	idGroups.Clear();
	binSpans.Clear();

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

uint ShaderRegistryBuilder::GetOrAddShader(const ShaderDef& shader) { return GetOrAddValue(shader, shaders); }

uint ShaderRegistryBuilder::GetOrAddEffect(const EffectDef& effect) { return GetOrAddValue(effect, effects); }

uint ShaderRegistryBuilder::GetOrAddIDGroup(const IDynamicArray<uint>& idGroup) { return GetOrAddValue(idGroup, idGroups); }

uint ShaderRegistryBuilder::GetOrAddShaderBin(const IDynamicArray<byte>& byteCode) { return GetOrAddValue(byteCode, binSpans); }

// Getter helpers
const ConstDef& ShaderRegistryBuilder::GetConstant(const uint id) const { return constants.GetValue(id); }

const ConstBufDef& ShaderRegistryBuilder::GetConstantBuffer(const uint id) const { return cbufDefs.GetValue(id); }

const IOElementDef& ShaderRegistryBuilder::GetIOElement(const uint id) const { return ioElements.GetValue(id); }

const ResourceDef& ShaderRegistryBuilder::GetResource(const uint id) const { return resources.GetValue(id); }

const ShaderDef& ShaderRegistryBuilder::GetShader(const uint id) const { return shaders.GetValue(id); }

const EffectDef& ShaderRegistryBuilder::GetEffect(const uint id) const { return effects.GetValue(id); }

IDSpan ShaderRegistryBuilder::GetIDGroup(const uint id) const { return idGroups.GetValue(id); }

ByteSpan ShaderRegistryBuilder::GetShaderBin(const uint id) const { return binSpans.GetValue(id); }

Vector<uint> ShaderRegistryBuilder::GetTmpIDBuffer() { return idBufPool.Get(); }

Vector<byte> ShaderRegistryBuilder::GetTmpByteBuffer() { return byteCodePool.Get(); }

void ShaderRegistryBuilder::ReturnTmpByteBuffer(Vector<byte>&& buf) { buf.Clear(); byteCodePool.Return(std::move(buf)); }

const StringIDBuilder& ShaderRegistryBuilder::GetStringIDBuilder() const { return stringIDs; }

void ShaderRegistryBuilder::ReturnTmpIDBuffer(Vector<uint>&& buf) { buf.Clear(); idBufPool.Return(std::move(buf)); }

ShaderRegistryDef::Handle ShaderRegistryBuilder::GetDefinition() const
{
	FX_ASSERT_MSG((idBufPool.GetObjectsOutstanding() + byteCodePool.GetObjectsOutstanding()) == 0,
		"Temporary buffers must be returned before finalizing or exporting shader regsitry.");

	return
	{
		.pConstants = &constants,
		.pCBufDefs = &cbufDefs,
		.pIOElements = &ioElements,
		.pResources = &resources,
		.pShaders = &shaders,
		.pEffects = &effects,
		.pIDGroups = &idGroups,
		.pBinSpans = &binSpans
	};
}

uint ShaderRegistryBuilder::SetResourceType(uint index, ResourceType type) { return ((index & 0x00FFFFFFu) | ((uint)type << 24u)); }

// ID/Index utils
ResourceType ShaderRegistryBuilder::GetResourceType(uint id) { return (ResourceType)(id >> 24u); }

uint ShaderRegistryBuilder::GetIndex(uint id) { return (id & 0x00FFFFFFu); }

// Heterogeneous hashing handle
HashHandle::HashHandle() : id(-1), pGetHash(nullptr), pGetIsEqual(nullptr)
{
}

HashHandle::HashHandle(const uint id, const HashFunc& getHash, const IsEqFunc& getIsEq) :
	id(id), pGetHash(&getHash), pGetIsEqual(&getIsEq)
{
}

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
