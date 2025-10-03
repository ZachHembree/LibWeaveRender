#include "pch.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "WeaveUtils/StringIDMap.hpp"

using namespace Weave;
using namespace Weave::Effects;

// ConstBuf
ConstBufDefHandle::ConstBufDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ConstBufDefHandle::ConstBufDefHandle(const ShaderRegistryMap & map, uint bufID) :
	pMap(&map), pDef(&map.GetConstBuf(bufID))
{ }

uint ConstBufDefHandle::GetNameID() const { return pDef->stringID; }

const StringSpan ConstBufDefHandle::GetName() const { return pMap->GetString(pDef->stringID); }

uint ConstBufDefHandle::GetSize() const { return pDef->size; }

const ConstDef& ConstBufDefHandle::operator[](ptrdiff_t index) const 
{ 
	IDSpan members = pMap->GetIDGroup(pDef->layoutID);
	return pMap->GetConstant(members[index]);
}

size_t ConstBufDefHandle::GetLength() const 
{ 
	IDSpan members = pMap->GetIDGroup(pDef->layoutID);
	return members.GetLength();
}

const IStringIDMap& ConstBufDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// ShaderDef
ShaderDefHandle::ShaderDefHandle() :
	pMap(nullptr), pDef(nullptr)
{ }

ShaderDefHandle::ShaderDefHandle(const ShaderRegistryMap& map, uint shaderID) :
	pMap(&map), pDef(&map.GetShader(shaderID))
{ }

uint ShaderDefHandle::GetFilePathID() const { return pDef->fileStringID; }

const StringSpan ShaderDefHandle::GetFilePath() const { return pMap->GetString(pDef->fileStringID); }

uint ShaderDefHandle::GetNameID() const { return pDef->nameID; }

const StringSpan ShaderDefHandle::GetName() const { return pMap->GetString(pDef->nameID); }

ByteSpan ShaderDefHandle::GetBinSrc() const { return pMap->GetByteCode(pDef->byteCodeID); }

ShadeStages ShaderDefHandle::GetStage() const { return pDef->stage; }

tvec3<uint> ShaderDefHandle::GetThreadGroupSize() const { return pDef->threadGroupSize; }

std::optional<IOLayoutHandle> ShaderDefHandle::GetInLayout() const
{ 
	if (pDef->inLayoutID != -1)
		return IOLayoutHandle(*pMap, pDef->inLayoutID);
	else
		return std::nullopt;
}

std::optional<IOLayoutHandle> ShaderDefHandle::GetOutLayout() const 
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

std::optional<ConstBufGroupHandle> ShaderDefHandle::GetConstantBuffers() const
{ 
	if (pDef->cbufGroupID != -1)
		return ConstBufGroupHandle(*pMap, pDef->cbufGroupID);
	else
		return std::nullopt;
}

const ShaderDef& ShaderDefHandle::GetDefinition() const { return *pDef; }

const ShaderRegistryMap& ShaderDefHandle::GetRegistry() const { return *pMap; }

const IStringIDMap& ShaderDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// EffectDef
EffectDefHandle::EffectDefHandle() :
	pMap(nullptr), def({})
{}

EffectDefHandle::EffectDefHandle(const ShaderRegistryMap& map, uint effectID) :
	pMap(&map), def(map.GetEffect(effectID)), passes(map.GetIDGroup(def.passGroupID))
{}

uint EffectDefHandle::GetNameID() const { return def.nameID; }

const StringSpan EffectDefHandle::GetName() const { return pMap->GetString(def.nameID); }

ShaderDefHandle EffectDefHandle::GetShader(int pass, int shader) const
{
	IDSpan shaders = pMap->GetIDGroup(passes[pass]);
	return ShaderDefHandle(*pMap, shaders[shader]);
}

IDSpan EffectDefHandle::GetPass(int pass) const
{ 
	return pMap->GetIDGroup(passes[pass]);
}

uint EffectDefHandle::GetShaderCount(int pass) const 
{ 
	IDSpan shaders = pMap->GetIDGroup(passes[pass]);
	return (uint)shaders.GetLength();
}

uint EffectDefHandle::GetPassCount() const 
{
	return (uint)passes.GetLength();
}

const EffectDef& EffectDefHandle::GetDefinition() const { return def; }

const ShaderRegistryMap& EffectDefHandle::GetRegistry() const { return *pMap; }

const IStringIDMap& EffectDefHandle::GetStringMap() const { return pMap->GetStringMap(); }

// IO layout
IOLayoutHandle::IOLayoutHandle() :
	pMap(nullptr), layout()
{ }

IOLayoutHandle::IOLayoutHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), layout(map.GetIDGroup(layoutID))
{ }

const IOElementDef& IOLayoutHandle::operator[](const ptrdiff_t index) const { return pMap->GetIOElement(layout[index]); }

size_t IOLayoutHandle::GetLength() const { return layout.GetLength(); }

const IStringIDMap& IOLayoutHandle::GetStringMap() const { return pMap->GetStringMap(); }

// Resource layout
ResourceGroupHandle::ResourceGroupHandle() :
	pMap(nullptr)
{ }

ResourceGroupHandle::ResourceGroupHandle(const ShaderRegistryMap& map, uint layoutID) :
	pMap(&map), layout(map.GetIDGroup(layoutID))
{ }

const ResourceDef& ResourceGroupHandle::operator[](const ptrdiff_t index) const { return pMap->GetResource(layout[index]); }

size_t ResourceGroupHandle::GetLength() const { return layout.GetLength(); }

const IStringIDMap& ResourceGroupHandle::GetStringMap() const { return pMap->GetStringMap(); }

// Cbuf group
ConstBufGroupHandle::ConstBufGroupHandle() :
	pMap(nullptr)
{ }

ConstBufGroupHandle::ConstBufGroupHandle(const ShaderRegistryMap& map, uint groupID) :
	pMap(&map), layout(map.GetIDGroup(groupID))
{}

ConstBufDefHandle ConstBufGroupHandle::operator[](const ptrdiff_t index) const { return ConstBufDefHandle(*pMap, layout[index]); }

/// <summary>
/// Returns the total number of buffers in the group
/// </summary>
size_t ConstBufGroupHandle::GetLength() const { return layout.GetLength(); }

const IStringIDMap& ConstBufGroupHandle::GetStringMap() const { return pMap->GetStringMap(); }