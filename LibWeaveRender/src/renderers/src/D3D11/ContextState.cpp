#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Viewport.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"

using namespace Weave::D3D11;

using ConflictState = ContextState::ConflictState;
using RWResourceUsages = ContextState::RWResourceUsages;

static constexpr std::array<RWResourceUsages, g_ShadeStageCount> s_NullUsage = {};
static constexpr ConflictState s_NullConflict = {};
static constexpr std::array<string_view, 4> s_RWUsageNames
{
	"ShaderResourceView",
	"UnorderedAccessView",
	"RenderTargetView",
	"DepthStencilView"
};

template<typename T>
const Span<T> GetConstSpan(const T* pStart, size_t length)
{
	return Span(const_cast<T*>(pStart), length);
}

template<typename T>
const Span<T> GetVariableSpan(const IDynamicArray<T>& src, uint maxLen, sint offset, uint extent)
{
	if (extent == (uint)-1)
	{
		extent = maxLen;
		offset = 0;
	}

	return GetConstSpan(&src[offset], extent - offset);
}

string_view ContextState::GetUsageName(RWResourceUsages usage)
{
	for (uint i = 0; i < 4; i++)
	{
		const uint flag = 1u << (i + 3);

		if (((uint)usage & flag) == flag)
			return s_RWUsageNames[i];
	}

	return "Unknown";
}

ContextState::ContextState() :
	topology(PrimTopology::UNDEFINED),
	pInputLayout(nullptr),
	pIndexBuffer(nullptr),
	pDepthStencil(nullptr),
	pBlendState(nullptr),
	pRasterState(nullptr),
	rtCount(0),
	vpCount(0),
	vertBufCount(0),
	uavCount(0),
	isInitialized(false),
	drawCount(0)
{}

ContextState::ContextState(ContextState&& other) noexcept = default;

ContextState& ContextState::operator=(ContextState&& rhs) noexcept = default;

ContextState::~ContextState() = default;

ContextState::StageState::StageState() :
	stage(ShadeStages::Unknown),
	pShader(nullptr),
	samplers(D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullptr),
	cbuffers(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, nullptr),
	srvs(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullptr),
	sampCount(0),
	cbufCount(0),
	srvCount(0),
	drawCount(0)
{}

void ContextState::StageState::Reset()
{
	pShader = nullptr;
	SetArrNull(samplers);
	SetArrNull(cbuffers);
	SetArrNull(srvs);
}

void ContextState::Init()
{
	rtvs = UniqueArray<IRenderTarget*>(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr);
	viewports = UniqueArray<Viewport>(D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	vertexBuffers = UniqueArray<ID3D11Buffer*>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, nullptr);
	vbStrides = UniqueArray<uint>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, 0);
	vbOffsets = UniqueArray<uint>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, 0);
	uavs = UniqueArray<IUnorderedAccess*>(D3D11_1_UAV_SLOT_COUNT, nullptr);

	stages = UniqueArray<StageState>(g_ShadeStageCount);

	for (uint i = 0; i < g_ShadeStageCount; i++)
		stages[i].stage = (ShadeStages)i;

	isInitialized = true;
}

void ContextState::Reset()
{
	topology = PrimTopology::UNDEFINED;
	pInputLayout = nullptr;
	pIndexBuffer = nullptr;
	pDepthStencil = nullptr;
	pBlendState = nullptr;
	pRasterState = nullptr;

	for (StageState& stage : stages)
		stage.Reset();

	SetArrNull(rtvs);
	rtCount = 0;

	SetArrNull(viewports);
	vpCount = 0;

	SetArrNull(vertexBuffers);
	SetArrNull(vbStrides);
	SetArrNull(vbOffsets);
	vertBufCount = 0;

	SetArrNull(uavs);
	uavCount = 0;
	
	conflictBuffer.Clear();
	resUsageMap.clear();
}

bool ContextState::GetIsValid() const { return isInitialized; }

ulong ContextState::GetDrawCount() const { return drawCount; }

void ContextState::IncrementDraw() { drawCount++; }

const ContextState::StageState& ContextState::GetStage(ShadeStages stage) const { return stages[(int)stage]; }

const Span<IRenderTarget*> ContextState::GetRenderTargets() const { return GetConstSpan(&rtvs[0], rtCount); }

IDepthStencil* ContextState::GetDepthStencil() const { return pDepthStencil; }

const Span<Viewport> ContextState::GetViewports() { return Span(viewports.GetData(), vpCount); }

ID3D11Buffer* ContextState::GetIndexBuffer() const { return pIndexBuffer; }

ID3D11InputLayout* ContextState::GetInputLayout() const { return pInputLayout; }

const Span<ID3D11Buffer*> ContextState::GetVertexBuffers() const { return GetConstSpan(vertexBuffers.GetData(), vertBufCount); }

const Span<uint> ContextState::GetVertStrides() const { return GetConstSpan(vbStrides.GetData(), vertBufCount); }

const Span<uint> ContextState::GetVertOffsets() const { return GetConstSpan(vbOffsets.GetData(), vertBufCount); }

bool ContextState::TryUpdateTopology(PrimTopology topology)
{
	if (this->topology != topology)
	{
		this->topology = topology;
		return true;
	}
	else
		return false;
}

uint ContextState::TryUpdateVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, sint startSlot)
{
	Span vbState(&vertexBuffers[startSlot], vertBuffers.GetLength());
	const uint newCount = (uint)vertBuffers.GetLength();
	uint extent = 0;

	for (uint i = 0; i < newCount; i++)
	{
		ID3D11Buffer* pVB = vertBuffers[i].Get();

		if (pVB != vbState[i])
		{
			vbState[i] = pVB;
			extent = startSlot + i + 1;
		}
	}

	if (extent != -1)
	{
		Span strideState(&vbStrides[startSlot], newCount);
		Span offsetState(&vbOffsets[startSlot], newCount);
		vertBufCount = std::max(vertBufCount, (uint)(startSlot + newCount));

		for (uint i = 0; i < newCount; i++)
		{
			strideState[i] = vertBuffers[i].GetStride();
			offsetState[i] = vertBuffers[i].GetOffset();
		}
	}

	return extent;
}

uint ContextState::TryResetVertexBuffers(sint startSlot, uint count)
{
	if (vertBufCount == 0 || count == 0)
		return 0;

	if (count == (uint)-1)
	{
		startSlot = 0u;
		count = vertBufCount;
	}

	SetArrNull(vertexBuffers, startSlot, count);
	SetArrNull(vbStrides, startSlot, count);
	SetArrNull(vbOffsets, startSlot, count);

	if ((startSlot + count) == vertBufCount)
		vertBufCount = startSlot;

	return startSlot + count;
}

bool ContextState::TryUpdateIndexBuffer(ID3D11Buffer* pIndexBuffer)
{
	if (this->pIndexBuffer != pIndexBuffer)
	{
		this->pIndexBuffer = pIndexBuffer;
		return true;
	}
	else
		return false;
}

bool ContextState::TryUpdateInputLayout(ID3D11InputLayout* pLayout)
{
	if (pInputLayout != pLayout)
	{
		pInputLayout = pLayout;
		return true;
	}
	else
		return false;
}

const Span<const Sampler*> ContextState::StageState::GetSamplers(sint offset, uint extent) const
{
	return GetVariableSpan(samplers, sampCount, offset, extent);
}

const Span<ID3D11Buffer*> ContextState::StageState::GetCBuffers(sint offset, uint extent) const
{
	return GetVariableSpan(cbuffers, cbufCount, offset, extent);
}

const Span<const IShaderResource*> ContextState::StageState::GetSRVs(sint offset, uint extent) const
{
	return GetVariableSpan(srvs, srvCount, offset, extent);
}

const Span<IUnorderedAccess*> ContextState::CSGetUAVs(sint offset, uint extent) const
{
	return GetVariableSpan(uavs, uavCount, offset, extent);
}

ID3D11DepthStencilView* ContextState::GetDepthStencilView() const { return pDepthStencil != nullptr ? pDepthStencil->GetDSV() : nullptr; }

ID3D11DepthStencilState* ContextState::GetDepthStencilState() const { return pDepthStencil != nullptr ? pDepthStencil->GetState() : nullptr; }

vec2 ContextState::GetDepthStencilRange() const { return pDepthStencil != nullptr ? pDepthStencil->GetRange() : vec2(0); }

bool ContextState::TrySetShader(ShadeStages stage, const ShaderVariantBase* pShader)
{
	ContextState::StageState& ss = stages[(uint)stage];
	ss.drawCount = drawCount;

	if (pShader != ss.pShader)
	{
		ss.pShader = pShader;
		return true;
	}
	else
		return false;
}

template<RWResourceUsages UsageT, typename ResT>
void ContextState::UpdateUsageMap(ShadeStages stage, const Span<ResT*> stateRes, const IDynamicArray<ResT*>& newRes)
{
	// Reset usage on changed slots
	for (uint slot = 0; slot < stateRes.GetLength(); slot++)
	{
		const ResT* pNext = (slot < newRes.GetLength()) ? newRes[slot] : nullptr;

		if (stateRes[slot] != nullptr && stateRes[slot] != pNext)
		{
			const auto it = resUsageMap.find(stateRes[slot]);

			if (it != resUsageMap.end())
			{
				// Clear last usage to avoid erroneous conflicts with incoming resources
				UsageDesc& desc = it->second;
				desc.ResetUsage(stage);

				if (desc.IsEmpty())
					resUsageMap.erase(stateRes[slot]);
			}
		}
	}

	// Set usage on new range
	for (uint slot = 0; slot < newRes.GetLength(); slot++)
	{
		if (newRes[slot] != nullptr)
		{
			const auto it = resUsageMap.find(newRes[slot]);

			// Add usage to existing description and buffer any resulting conflicts
			if (it != resUsageMap.end())
			{
				UsageDesc& desc = it->second;
				desc.SetUsage(stage, UsageT, slot, conflictBuffer);
			}
			else
				resUsageMap.emplace(newRes[slot], UsageDesc(stage, UsageT, slot));
		}
	}
}

template<typename ResT>
static uint UpdateResources(IDynamicArray<ResT>& stateRes, uint& stateCount, const IDynamicArray<ResT>& newRes, sint startSlot = 0)
{
	const uint oldCount = stateCount;
	const uint newCount = (uint)newRes.GetLength() + startSlot;
	const uint updateExtent = std::max(oldCount, newCount);
	stateCount = newCount;

	// Write new resources to state cache
	Span dstSpan(&stateRes[startSlot], newRes.GetLength());
	ArrMemCopy(dstSpan, newRes);

	// Set extra range to null
	if (oldCount > newCount)
		SetArrNull(stateRes, newCount, oldCount - newCount);

	return updateExtent;
}

bool ContextState::TryUpdateRenderTargets(IDynamicArray<IRenderTarget*>& newRes, uint startSlot)
{
	if ((startSlot < rtCount) && Span<IRenderTarget*>(&rtvs[startSlot], rtCount - startSlot) == newRes)
		return false;

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::RenderTargetView>(ShadeStages::Pixel, Span(&rtvs[startSlot], newRes.GetLength()), newRes);

	return (UpdateResources(rtvs, rtCount, newRes, startSlot) > 0);
}

bool ContextState::TryUpdateViewports()
{
	const vec2 depthRange = GetDepthStencilRange();
	bool wasChanged = false;

	for (uint i = 0; i < rtCount; i++)
	{
		if (rtvs[i] == nullptr)
			continue;

		const IRenderTarget& rt = *rtvs[i];
		Viewport vp
		{
			.offset = rt.GetRenderOffset(),
			.size = rt.GetRenderSize(),
			.zDepth = depthRange
		};

		if (vp != viewports[i])
		{
			viewports[i] = vp;
			wasChanged = true;
		}
	}

	vpCount = rtCount;
	return wasChanged;
}

bool ContextState::TryUpdateDepthStencil(IDepthStencil* pDepthStencil)
{
	if (this->pDepthStencil == pDepthStencil)
		return false;

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::DepthStencilView>(ShadeStages::Pixel, Span(&this->pDepthStencil), Span(&pDepthStencil));

	this->pDepthStencil = pDepthStencil;
	return true;
}

const Span<ID3D11Buffer*> ContextState::TryUpdateResources(ShadeStages stage, IDynamicArray<ConstantBuffer>& cbufs)
{
	ContextState::StageState& ss = stages[(uint)stage];
	Span<ID3D11Buffer*> newCbufs;

	if (cbufs.GetLength() > 0)
	{
		ALLOCA_SPAN_SET(newCbufs, cbufs.GetLength(), ID3D11Buffer*);

		// Retrieve D3D pointers
		for (uint i = 0; i < cbufs.GetLength(); i++)
			newCbufs[i] = cbufs[i].Get();
	}

	const uint extent = UpdateResources(ss.cbuffers, ss.cbufCount, newCbufs);
	return Span(ss.cbuffers.GetData(), extent);
}

const Span<const Sampler*> ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SamplerList& resSrc, const SamplerMap* pResMap)
{
	ContextState::StageState& ss = stages[(uint)stage];
	Span<const Sampler*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), const Sampler*);
		pResMap->GetResources(resSrc, newRes);
	}

	// Populate the state cache based on the mapped resources
	const uint extent = UpdateResources(ss.samplers, ss.sampCount, newRes);
	return Span(ss.samplers.GetData(), extent);
}

const Span<const IShaderResource*> ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SRVList& resSrc, const ResourceViewMap* pResMap)
{
	ContextState::StageState& ss = stages[(uint)stage];
	Span<const IShaderResource*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), const IShaderResource*);
		pResMap->GetResources(resSrc, newRes);
	}

	if (Span<const IShaderResource*>(ss.srvs.GetData(), ss.srvCount) == newRes)
		return {};

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::ShaderResourceView>(stage, Span(ss.srvs.GetData(), ss.srvCount), newRes);

	// Populate the state cache based on the mapped resources
	const uint extent = UpdateResources(ss.srvs, ss.srvCount, newRes);
	return Span(ss.srvs.GetData(), extent);
}

const Span<IUnorderedAccess*> ContextState::TryUpdateResources(const ResourceSet::UAVList& resSrc, const UnorderedAccessMap* pResMap)
{
	Span<IUnorderedAccess*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), IUnorderedAccess*);
		pResMap->GetResources(resSrc, newRes);
	}

	if (Span<IUnorderedAccess*>(uavs.GetData(), uavCount) == newRes)
		return {};

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::UnorderedAccessView>(ShadeStages::Compute, Span(uavs.GetData(), uavCount), newRes);

	// Populate the state cache based on the mapped resources
	const uint extent = UpdateResources(uavs, uavCount, newRes);
	return Span(uavs.GetData(), extent);
}

template<typename T>
static uint GetLastNonNull(const IDynamicArray<T*>& arr, uint start)
{
	for (int i = start; i >= 0; i--)
	{
		if (arr[i] == nullptr)
			start--;
		else
			return start;
	}

	return 0;
}

void ContextState::LogInvalidStateTransitions(RWConflictDesc conflict)
{
	StageState& conflictState = stages[(uint)conflict.lastStage];

	// No shader bound, conflict irrelevant
	if (conflictState.pShader == nullptr)
		return;

	const bool isWriting = (conflict.nextUsage & RWResourceUsages::Write) == RWResourceUsages::Write;
	// Conflict occured within same setup phase 
	const bool intraStageSetupErr = (conflict.nextStage != conflict.lastStage && conflictState.drawCount == drawCount);
	// Conflict occured within the same stage
	const bool stageSetupErr = (conflict.nextStage == conflict.lastStage && isWriting);

	if (intraStageSetupErr || stageSetupErr)
	{
		ShaderDefHandle lastShader = conflictState.pShader->GetDefinition();
		ShaderDefHandle nextShader = GetStage(conflict.nextStage).pShader->GetDefinition();
		string_view lastName = lastShader.GetStringMap().GetString(lastShader.GetNameID());
		string_view nextName = nextShader.GetStringMap().GetString(nextShader.GetNameID());

		WV_LOG_DEBUG() << "A conflicting resource usage was specified during the setup phase.\n"
			<< GetUsageName(conflict.lastUsage) << " (slot: " << conflict.slot << ")"
			<< " in " << lastName << " stage: " << GetStageName(conflict.lastStage)
			<< " conflicts with " << GetUsageName(conflict.nextUsage)
			<< " in " << nextName << " stage: " << GetStageName(conflict.nextStage);
	}
}

ContextState::ConflictState ContextState::TryResolveConflicts()
{
	ConflictState state = {};

	if (conflictBuffer.GetLength() > 0)
	{
		// Set cached conflicts to null in preparation for context update
		for (const RWConflictDesc& conflict : conflictBuffer)
		{
			// Log errors
			LogInvalidStateTransitions(conflict);

			// Correct internal state
			switch (conflict.lastUsage)
			{
			case RWResourceUsages::ShaderResourceView:
			{			
				StageState& ss = stages[(uint)conflict.lastStage];
				ss.srvs[conflict.slot] = nullptr;
				state.srvsExtent[(uint)conflict.lastStage] = (byte)ss.srvCount;
				break;
			}
			case RWResourceUsages::UnorderedAccessView:
			{
				uavs[conflict.slot] = nullptr;
				state.uavsExtent = (byte)uavCount;
				break;
			}
			case RWResourceUsages::RenderTargetView:
				rtvs[conflict.slot] = nullptr;
				state.rtvExtent = (byte)rtCount;
				break;
			case RWResourceUsages::DepthStencilView:
				pDepthStencil = nullptr;
				state.rtvExtent = (byte)rtCount;
				break;
			}
		}

		// Update internal resource counts
		for (uint i = 0; i < g_ShadeStageCount; i++)
		{
			if (state.srvsExtent[i] != 0)
				stages[i].srvCount = GetLastNonNull(stages[i].srvs, stages[i].srvCount);
		}

		if (state.uavsExtent != 0)
			uavCount = GetLastNonNull(uavs, uavCount);

		if (state.rtvExtent != 0)
			rtCount = GetLastNonNull(rtvs, rtCount);
	}

	// Return counters and reset
	conflictBuffer.Clear();
	return state;
}

ContextState::UsageDesc::UsageDesc() :
	uses({}),
	slots({})
{ }

ContextState::UsageDesc::UsageDesc(ShadeStages stage, RWResourceUsages usage, uint slot) :
	uses({}),
	slots({})
{
	uses[(uint)stage] = usage;
	slots[(uint)stage] = (byte)slot;
}

uint ContextState::UsageDesc::GetSlot(ShadeStages stage) const { return (uint)slots[(uint)stage]; }

RWResourceUsages ContextState::UsageDesc::GetUsage(ShadeStages stage) const { return uses[(uint)stage]; }

uint ContextState::UsageDesc::SetUsage(ShadeStages stage, RWResourceUsages usage, uint slot, Vector<RWConflictDesc>& conflictBuffer)
{
	uint conflictCount = 0;
	D3D_ASSERT(!IsEmpty());

	const bool isWriting = (usage & RWResourceUsages::Write) == RWResourceUsages::Write;

	for (uint i = 0; i < g_ShadeStageCount; i++)
	{
		const RWResourceUsages lastUsage = uses[i];
		const ShadeStages lastStage = (ShadeStages)i;
		const uint lastSlot = (uint)slots[i];
		const bool wasUsed = lastUsage != RWResourceUsages::Unused;
		const bool wasWriting = (lastUsage & RWResourceUsages::Write) == RWResourceUsages::Write;

		// Is reading and was writing, or is writing and was reading
		// W -> R or R -> W
		if (wasUsed && (isWriting != wasWriting))
		{
			conflictBuffer.emplace_back(RWConflictDesc
			{
				.nextStage = stage,
				.nextUsage = usage,
				.lastStage = lastStage,
				.lastUsage = lastUsage,
				.slot = lastSlot
			});

			uses[(uint)lastStage] = RWResourceUsages::Unused;
			conflictCount++;
		}
	}

	uses[(uint)stage] = usage;
	slots[(uint)stage] = (byte)slot;
	return conflictCount;
}

void ContextState::UsageDesc::ResetUsage(ShadeStages stage) { uses[(uint)stage] = RWResourceUsages::Unused; }

bool ContextState::UsageDesc::IsEmpty() const { return (memcmp(&uses, &s_NullUsage, sizeof(uses)) == 0); }

bool ContextState::ConflictState::IsEmpty() const { return (memcmp(this, &s_NullConflict, sizeof(ConflictState)) == 0); }
