#include "pch.hpp"
#include "ReplicaEffects/ShaderDataHandles.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "ReplicaUtils/StringIDMap.hpp"

using namespace Replica;
using namespace Replica::Effects;

// ConstBuf
ConstBufDefHandle::ConstBufDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ConstBufDefHandle::ConstBufDefHandle(const ShaderRegistryMap & map, uint bufID) :
	pMap(&map), pDef(&map.GetConstBuf(bufID))
{ }

uint ConstBufDefHandle::GetNameID() const { return pDef->stringID; }

uint ConstBufDefHandle::GetSize() const { return pDef->size; }

const ConstDef& ConstBufDefHandle::operator[](ptrdiff_t index) const 
{ 
	const IDynamicArray<uint>& members = pMap->GetCBufLayout(pDef->layoutID);
	return pMap->GetConstant(members[index]);
}

size_t ConstBufDefHandle::GetLength() const 
{ 
	const IDynamicArray<uint>& members = pMap->GetCBufLayout(pDef->layoutID);
	return members.GetLength();
}

const StringIDMap& ConstBufDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// ShaderDef
ShaderDefHandle::ShaderDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ShaderDefHandle::ShaderDefHandle(const ShaderRegistryMap& map, uint shaderID) :
	pMap(&map), pDef(&map.GetShader(shaderID))
{ }

uint ShaderDefHandle::GetFilePathID() const { return pDef->fileStringID; }

uint ShaderDefHandle::GetNameID() const { return pDef->nameID; }

const IDynamicArray<byte>& ShaderDefHandle::GetBinSrc() const { return pMap->GetByteCode(pDef->byteCodeID); }

ShadeStages ShaderDefHandle::GetStage() const { return pDef->stage; }

tvec3<uint> ShaderDefHandle::GetThreadGroupSize() const { return pDef->threadGroupSize; }

std::optional <IOLayoutHandle> ShaderDefHandle::GetInLayout() const
{ 
	if (pDef->inLayoutID != -1)
		return IOLayoutHandle(*pMap, pDef->inLayoutID);
	else
		return std::nullopt;
}

std::optional <IOLayoutHandle> ShaderDefHandle::GetOutLayout() const 
{ 
	if (pDef->outLayoutID != -1)
		return IOLayoutHandle(*pMap, pDef->outLayoutID);
	else
		return std::nullopt;
}

std::optional<ResourceGroupHandle> ShaderDefHandle::GetResources() const
{ 
	if (pDef->resLayoutID != -1)
		return ResourceGroupHandle(*pMap, pDef->resLayoutID);
	else
		return std::nullopt;
}

std::optional <ConstBufGroupHandle> ShaderDefHandle::GetConstantBuffers() const
{ 
	if (pDef->cbufGroupID != -1)
		return ConstBufGroupHandle(*pMap, pDef->cbufGroupID);
	else
		return std::nullopt;
}

const StringIDMap& ShaderDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// EffectDef
EffectDefHandle::EffectDefHandle() :
	pMap(nullptr), pDef(nullptr)
{}

EffectDefHandle::EffectDefHandle(const ShaderRegistryMap& map, uint effectID) :
	pMap(&map), pDef(&map.GetEffect(effectID))
{}

uint EffectDefHandle::GetNameID() const { return pDef->nameID; }

ShaderDefHandle EffectDefHandle::GetShader(int pass, int shader) const
{
	const IDynamicArray<uint>& shaders = pMap->GetEffectPass(pDef->passes[pass]);
	return ShaderDefHandle(*pMap, shaders[shader]);
}

const IDynamicArray<uint>& EffectDefHandle::GetPass(int pass) const 
{ 
	return pMap->GetEffectPass(pDef->passes[pass]); 
}

uint EffectDefHandle::GetShaderCount(int pass) const 
{ 
	const IDynamicArray<uint>& shaders = pMap->GetEffectPass(pDef->passes[pass]);
	return (uint)shaders.GetLength();
}

uint EffectDefHandle::GetPassCount() const { return (uint)pDef->passes.GetLength(); }

const StringIDMap& EffectDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// IO layout
IOLayoutHandle::IOLayoutHandle() :
	pMap(nullptr), pLayout(nullptr)
{ }

IOLayoutHandle::IOLayoutHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), pLayout(&map.GetIOLayout(layoutID))
{ }

const IOElementDef& IOLayoutHandle::operator[](const ptrdiff_t index) const { return pMap->GetIOElement(pLayout->operator[](index)); }

size_t IOLayoutHandle::GetLength() const { return pLayout->GetLength(); }

const StringIDMap& IOLayoutHandle::GetStringMap() const { return pMap->GetStringMap(); }

// Resource layout
ResourceGroupHandle::ResourceGroupHandle() :
	pMap(nullptr), pLayout(nullptr)
{ }

ResourceGroupHandle::ResourceGroupHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), pLayout(&map.GetResourceGroup(layoutID))
{ }

const ResourceDef& ResourceGroupHandle::operator[](const ptrdiff_t index) const { return pMap->GetResource(pLayout->operator[](index)); }

size_t ResourceGroupHandle::GetLength() const { return pLayout->GetLength(); }

const StringIDMap& ResourceGroupHandle::GetStringMap() const { return pMap->GetStringMap(); }

// Cbuf group
ConstBufGroupHandle::ConstBufGroupHandle() :
	pMap(nullptr), pLayout(nullptr)
{ }

ConstBufGroupHandle::ConstBufGroupHandle(const ShaderRegistryMap& map, uint groupID) :
	pMap(&map), pLayout(&map.GetCBufGroup(groupID))
{}

ConstBufDefHandle ConstBufGroupHandle::operator[](const ptrdiff_t index) const { return ConstBufDefHandle(*pMap, pLayout->operator[](index)); }

/// <summary>
/// Returns the total number of buffers in the group
/// </summary>
size_t ConstBufGroupHandle::GetLength() const { return pLayout->GetLength(); }

const StringIDMap& ConstBufGroupHandle::GetStringMap() const { return pMap->GetStringMap(); }