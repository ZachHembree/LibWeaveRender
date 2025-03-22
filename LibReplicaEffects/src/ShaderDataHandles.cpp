#include "pch.hpp"
#include "ShaderDataHandles.hpp"
#include "ShaderLibGen/ShaderRegistryMap.hpp"

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

const ConstDef& ConstBufDefHandle::operator[](ptrdiff_t index) const { return pMap->GetConstant(pDef->members[index]); }

size_t ConstBufDefHandle::GetLength() const { return pDef->members.GetLength(); }

// ShaderDef
ShaderDefHandle::ShaderDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ShaderDefHandle::ShaderDefHandle(const ShaderRegistryMap& map, uint shaderID) :
	pMap(&map), pDef(&map.GetShader(shaderID))
{ }

uint ShaderDefHandle::GetFilePathID() const { return pDef->fileStringID; }

uint ShaderDefHandle::GetNameID() const { return pDef->nameID; }

const IDynamicArray<byte>& ShaderDefHandle::GetBinSrc() const { return pDef->binSrc; }

ShadeStages ShaderDefHandle::GetStage() const { return pDef->stage; }

tvec3<uint> ShaderDefHandle::GetThreadGroupSize() const { return pDef->threadGroupSize; }

IOLayoutHandle ShaderDefHandle::GetInLayout() const { return IOLayoutHandle(*pMap, pDef->inLayoutID); }

IOLayoutHandle ShaderDefHandle::GetOutLayout() const { return IOLayoutHandle(*pMap, pDef->outLayoutID); }

ResourceGroupHandle ShaderDefHandle::GetResources() const { return ResourceGroupHandle(*pMap, pDef->resLayoutID); }

ConstBufGroupHandle ShaderDefHandle::GetConstantBuffers() const { return ConstBufGroupHandle(*pMap, pDef->cbufGroupID); }

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
	const IDynamicArray<uint>& shaders = pDef->passes[pass].shaderIDs;
	return ShaderDefHandle(*pMap, shaders[shader]);
}

uint EffectDefHandle::GetShaderCount(int pass) const { return (uint)pDef->passes[pass].shaderIDs.GetLength(); }

uint EffectDefHandle::GetPassCount() const { return (uint)pDef->passes.GetLength(); }

// IO layout
IOLayoutHandle::IOLayoutHandle() :
	pMap(nullptr), pLayout(nullptr)
{ }

IOLayoutHandle::IOLayoutHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), pLayout(&map.GetIOLayout(layoutID))
{ }

const IOElementDef& IOLayoutHandle::operator[](const ptrdiff_t index) const { return pMap->GetIOElement(pLayout->operator[](index)); }

size_t IOLayoutHandle::GetLength() const { return pLayout->GetLength(); }

// Resource layout
ResourceGroupHandle::ResourceGroupHandle() :
	pMap(nullptr), pLayout(nullptr)
{ }

ResourceGroupHandle::ResourceGroupHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), pLayout(&map.GetResourceGroup(layoutID))
{ }

const ResourceDef& ResourceGroupHandle::operator[](const ptrdiff_t index) const { return pMap->GetResource(pLayout->operator[](index)); }

size_t ResourceGroupHandle::GetLength() const { return pLayout->GetLength(); }

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
