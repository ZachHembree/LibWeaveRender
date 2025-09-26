#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryBuilder.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"

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

	shaderCopyCache.clear();
	effectCopyCache.clear();
	pCopySrc = nullptr;
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

/// <summary>
/// Copies an IOLayout from from a ShaderRegistryMap and maps it into the given registry builder. Returns the layout ID 
/// for the new layout in the builder.
/// </summary>
static uint RemapIOLayout(const std::optional<IOLayoutHandle>& layout, ShaderRegistryBuilder& builder)
{
	uint layoutID = -1;

	if (layout.has_value())
	{
		Vector<uint> idBuf = builder.GetTmpIDBuffer();

		for (uint i = 0; i < layout->GetLength(); i++)
		{
			IOElementDef element = (*layout)[i];
			string_view name = layout->GetStringMap().GetString(element.semanticID);
			element.semanticID = builder.GetOrAddStringID(name);
			idBuf.EmplaceBack(builder.GetOrAddIOElement(element));
		}

		layoutID = builder.GetOrAddIDGroup(idBuf);
		builder.ReturnTmpIDBuffer(std::move(idBuf));
	}
	
	return layoutID;
}

/// <summary>
/// Remaps a group of constant buffers from another shader registry into the give registry builder
/// </summary>
static uint RemapConstants(const std::optional<ConstBufGroupHandle>& group, ShaderRegistryBuilder& builder)
{
	uint groupID = -1;

	if (group.has_value())
	{
		Vector<uint> groupIDbuf = builder.GetTmpIDBuffer();

		// Remap constant buffers
		for (uint i = 0; i < group->GetLength(); i++)
		{
			ConstBufDefHandle bufHandle = (*group)[i];

			Vector<uint> constIDbuf = builder.GetTmpIDBuffer();
			ConstBufDef bufDef;
			bufDef.stringID = builder.GetOrAddStringID(bufHandle.GetName());
			bufDef.size = bufHandle.GetSize();

			// Remap constants
			for (uint j = 0; j < bufHandle.GetLength(); j++)
			{
				ConstDef varDef = bufHandle[j];
				string_view varName = bufHandle.GetStringMap().GetString(varDef.stringID);
				varDef.stringID = builder.GetOrAddStringID(varName);
				constIDbuf.EmplaceBack(builder.GetOrAddConstant(varDef));
			}

			bufDef.layoutID = builder.GetOrAddIDGroup(constIDbuf);
			groupIDbuf.EmplaceBack(builder.GetOrAddConstantBuffer(bufDef));
			builder.ReturnTmpIDBuffer(std::move(constIDbuf));
		}

		groupID = !groupIDbuf.IsEmpty() ? builder.GetOrAddIDGroup(groupIDbuf) : -1;
		builder.ReturnTmpIDBuffer(std::move(groupIDbuf));
	}

	return groupID;
}

/// <summary>
/// Remaps a group of resources (textures/samplers), from another shader registry into the given builder
/// </summary>
static uint RemapResources(const std::optional<ResourceGroupHandle>& resources, ShaderRegistryBuilder& builder)
{
	uint layoutID = -1;

	if (resources.has_value())
	{
		Vector<uint> idBuf = builder.GetTmpIDBuffer();

		for (uint i = 0; i < resources->GetLength(); i++)
		{
			ResourceDef res = (*resources)[i];
			string_view resName = resources->GetStringMap().GetString(res.stringID);
			res.stringID = builder.GetOrAddStringID(resName);
			idBuf.EmplaceBack(builder.GetOrAddResource(res));
		}

		layoutID = !idBuf.IsEmpty() ? builder.GetOrAddIDGroup(idBuf) : -1;
		builder.ReturnTmpIDBuffer(std::move(idBuf));
	}

	return layoutID;
}

uint ShaderRegistryBuilder::GetOrAddShader(const ShaderDefHandle& shaderDef)
{
	// Evict copy cache if source changes
	if (&shaderDef.GetRegistry() != pCopySrc)
	{
		shaderCopyCache.clear();
		effectCopyCache.clear();
		pCopySrc = &shaderDef.GetRegistry();
	}
	
	// Attempt to remap from copy cache
	const auto& it = shaderCopyCache.find(shaderDef.GetDefinition());

	if (it != shaderCopyCache.end())
	{
		return it->second;
	}
	// Full remap
	else
	{
		ShaderDef cpy =
		{
			.fileStringID = GetOrAddStringID(shaderDef.GetFilePath()),
			.byteCodeID = GetOrAddShaderBin(shaderDef.GetBinSrc()),
			.nameID = GetOrAddStringID(shaderDef.GetName()),
			.stage = shaderDef.GetStage(),
			// Reflection
			.threadGroupSize = shaderDef.GetThreadGroupSize(),
			// Input/Output params
			.inLayoutID = RemapIOLayout(shaderDef.GetInLayout(), *this),
			.outLayoutID = RemapIOLayout(shaderDef.GetOutLayout(), *this),
			// Textures and resources
			.resLayoutID = RemapResources(shaderDef.GetResources(), *this),
			// Constants
			.cbufGroupID = RemapConstants(shaderDef.GetConstantBuffers(), *this)
		};

		const uint shaderID = GetOrAddShader(cpy);
		shaderCopyCache.emplace(shaderDef.GetDefinition(), shaderID);
		return shaderID;
	}
}

uint ShaderRegistryBuilder::GetOrAddEffect(const EffectDefHandle& effectDef)
{
	// Evict copy cache if source changes
	if (&effectDef.GetRegistry() != pCopySrc)
	{
		shaderCopyCache.clear();
		effectCopyCache.clear();
		pCopySrc = &effectDef.GetRegistry();
	}

	// Attempt to remap from copy cache
	const auto& it = effectCopyCache.find(effectDef.GetDefinition());

	if (it != effectCopyCache.end())
	{
		return it->second;
	}
	// Full remap
	else
	{
		// Remap dependencies
		Vector<uint> effectPasses = GetTmpIDBuffer();
		Vector<uint> passBuf = GetTmpIDBuffer();

		for (int i = 0; i < (int)effectDef.GetPassCount(); i++)
		{
			for (int j = 0; j < (int)effectDef.GetShaderCount(i); j++)
			{
				ShaderDefHandle shader = effectDef.GetShader(i, j);
				uint shaderID = GetOrAddShader(shader);
				passBuf.Add(shaderID);
			}

			effectPasses.Add(GetOrAddIDGroup(passBuf));
			passBuf.Clear();
		}

		// Finalize definition
		EffectDef cpy;
		cpy.nameID = GetOrAddStringID(effectDef.GetName());
		cpy.passGroupID = GetOrAddIDGroup(effectPasses);
		ReturnTmpIDBuffer(std::move(effectPasses));
		ReturnTmpIDBuffer(std::move(passBuf));

		// Cache result and return
		const uint effectID = GetOrAddEffect(cpy);
		effectCopyCache.emplace(cpy, effectID);
		return effectID;
	}
}

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
