#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Viewport.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"

using namespace Weave::D3D11;

using ConflictState = ContextState::ConflictState;
using RWResourceUsages = ContextState::RWResourceUsages;

static constexpr std::array<RWResourceUsages, g_ShadeStageCount> s_NullUsage = {};
static constexpr ConflictState s_NullConflict = {};

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
	isInitialized(false)
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
	srvCount(0)
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
	rtvs = UniqueArray<ID3D11RenderTargetView*>(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr);
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

ContextState::StageState& ContextState::GetStage(ShadeStages stage) { return stages[(int)stage]; }

ID3D11DepthStencilView* ContextState::GetDepthStencilView() const { return pDepthStencil != nullptr ? pDepthStencil->GetDSV() : nullptr; }

ID3D11DepthStencilState* ContextState::GetDepthStencilState() const { return pDepthStencil != nullptr ? pDepthStencil->GetState() : nullptr; }

vec2 ContextState::GetDepthStencilRange() const { return pDepthStencil != nullptr ? pDepthStencil->GetRange() : vec2(0); }

template<typename ResT>
uint UpdateResources(IDynamicArray<ResT*>& stateRes, uint& stateCount, const IDynamicArray<ResT*>& newRes)
{
	const uint oldCount = stateCount;
	const uint newCount = (uint)newRes.GetLength();
	const uint updateExtent = std::max(oldCount, newCount);
	stateCount = newCount;

	// Write new resources to state cache
	ArrMemCopy(stateRes, newRes);

	// Set extra range to null
	if (oldCount > newCount)
		SetArrNull(stateRes, newCount, oldCount - newCount);

	return updateExtent;
}

uint ContextState::TryUpdateResources(ShadeStages stage, IDynamicArray<ConstantBuffer>& cbufs)
{
	ContextState::StageState& ss = GetStage(stage);
	Span<ID3D11Buffer*> newCbufs;

	if (cbufs.GetLength() > 0)
	{
		ALLOCA_SPAN_SET(newCbufs, cbufs.GetLength(), ID3D11Buffer*);

		// Retrieve D3D pointers
		for (uint i = 0; i < cbufs.GetLength(); i++)
			newCbufs[i] = cbufs[i].Get();
	}

	return UpdateResources(ss.cbuffers, ss.cbufCount, newCbufs);
}

uint ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SamplerList& resSrc, const SamplerMap* pResMap)
{
	ContextState::StageState& ss = GetStage(stage);
	Span<Sampler*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), Sampler*);
		pResMap->GetResources(resSrc, newRes);
	}

	// Populate the state cache based on the mapped resources
	return UpdateResources(ss.samplers, ss.sampCount, newRes);
}

uint ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SRVList& resSrc, const ResourceViewMap* pResMap)
{
	ContextState::StageState& ss = GetStage(stage);
	Span<IShaderResource*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), IShaderResource*);
		pResMap->GetResources(resSrc, newRes);
	}

	if (Span<IShaderResource*>(ss.srvs.GetData(), ss.srvCount) == newRes)
		return 0;

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::ShaderResourceView>(stage, Span(ss.srvs.GetData(), ss.srvCount), newRes);

	// Populate the state cache based on the mapped resources
	return UpdateResources(ss.srvs, ss.srvCount, newRes);
}

uint ContextState::TryUpdateResources(const ResourceSet::UAVList& resSrc, const UnorderedAccessMap* pResMap)
{
	Span<IUnorderedAccess*> newRes;

	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN_SET(newRes, pResMap->GetCount(), IUnorderedAccess*);
		pResMap->GetResources(resSrc, newRes);
	}

	if (Span<IUnorderedAccess*>(uavs.GetData(), uavCount) == newRes)
		return 0;

	// Update and track usage
	UpdateUsageMap<RWResourceUsages::UnorderedAccessView>(ShadeStages::Compute, Span(uavs.GetData(), uavCount), newRes);

	// Populate the state cache based on the mapped resources
	return UpdateResources(uavs, uavCount, newRes);
}

template<typename T>
uint GetLastNonNull(const IDynamicArray<T*>& arr, uint start)
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

ContextState::ConflictState ContextState::TryResolveConflicts()
{
	ConflictState state = {};

	if (conflictBuffer.GetLength() > 0)
	{
		// Set cached conflicts to null in preparation for context update
		for (const RWConflictDesc& conflict : conflictBuffer)
		{
			switch (conflict.usage)
			{
			case RWResourceUsages::ShaderResourceView:
			{
				StageState& ss = GetStage(conflict.stage);
				ss.srvs[conflict.slot] = nullptr;
				state.srvsExtent[(uint)conflict.stage] = (byte)ss.srvCount;
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
				.stage = lastStage,
				.usage = lastUsage,
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
