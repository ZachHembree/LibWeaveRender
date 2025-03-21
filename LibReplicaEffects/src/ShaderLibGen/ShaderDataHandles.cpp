#include "pch.hpp"
#include "ShaderLibGen/ShaderDataHandles.hpp"
#include "ShaderLibGen/ShaderRegistryMap.hpp"

using namespace Replica;
using namespace Replica::Effects;

ConstBufDefHandle::ConstBufDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ConstBufDefHandle::ConstBufDefHandle(const ShaderRegistryMap & map, uint bufID) :
	pMap(&map), pDef(&map.GetConstBuf(bufID))
{ }

uint ConstBufDefHandle::GetNameID() const { return pDef->stringID; }

uint ConstBufDefHandle::GetSize() const { return pDef->size; }

const ConstDef& ConstBufDefHandle::GetConstant(int index) const { return pMap->GetConstant(pDef->members[index]); }

uint ConstBufDefHandle::GetConstantCount() const { return (uint)pDef->members.GetLength(); }

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

const IOElementDef& ShaderDefHandle::GetInElement(int index) const
{
	const IDynamicArray<uint>& layoutIDs = pMap->GetIOLayout(pDef->inLayoutID);
	return pMap->GetIOElement(layoutIDs[index]);
}

uint ShaderDefHandle::GetInLayoutLength() const { return (uint)pMap->GetIOLayout(pDef->inLayoutID).GetLength(); }

const IOElementDef& ShaderDefHandle::GetOutElement(int index) const
{
	const IDynamicArray<uint>& layoutIDs = pMap->GetIOLayout(pDef->outLayoutID);
	return pMap->GetIOElement(layoutIDs[index]);
}

uint ShaderDefHandle::GetOutLayoutLength() const { return (uint)pMap->GetIOLayout(pDef->outLayoutID).GetLength(); }

const ResourceDef& ShaderDefHandle::GetResource(int index) const
{
	const IDynamicArray<uint>& resIDs = pMap->GetResourceGroup(pDef->resLayoutID);
	return pMap->GetResource(resIDs[index]);
}

uint ShaderDefHandle::GetResourceCount() const { return (uint)pMap->GetResourceGroup(pDef->resLayoutID).GetLength(); }

ConstBufDefHandle ShaderDefHandle::GetConstantBuffer(int index) const
{
	const IDynamicArray<uint>& bufGroup = pMap->GetCBufGroup(pDef->cbufGroupID);
	return ConstBufDefHandle(*pMap, bufGroup[index]);
}

uint ShaderDefHandle::GetConstBufCount() const { return (uint)pMap->GetCBufGroup(pDef->cbufGroupID).GetLength(); }

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
