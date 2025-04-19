#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Viewport.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"

using namespace Weave::D3D11;

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
{ }

ContextState::ContextState(ContextState && other) noexcept = default;

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
	renderTargets = UniqueArray<ID3D11RenderTargetView*>(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr);
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

	SetArrNull(renderTargets);
	rtCount = 0;

	SetArrNull(viewports);
	vpCount = 0;

	SetArrNull(vertexBuffers);
	SetArrNull(vbStrides);
	SetArrNull(vbOffsets);
	vertBufCount = 0;

	SetArrNull(uavs);
	uavCount = 0;
}

bool ContextState::GetIsValid() const { return isInitialized; }

ContextState::StageState& ContextState::GetStage(ShadeStages stage) { return stages[(int)stage]; }

ID3D11DepthStencilView* ContextState::GetDepthStencilView() const { return pDepthStencil != nullptr ? pDepthStencil->GetDSV() : nullptr; }

ID3D11DepthStencilState* ContextState::GetDepthStencilState() const { return pDepthStencil != nullptr ? pDepthStencil->GetState() : nullptr; }

vec2 ContextState::GetDepthStencilRange() const { return pDepthStencil != nullptr ? pDepthStencil->GetRange() : vec2(0); }

template<typename ResT>
uint ContextState::TryUpdateResources(IDynamicArray<ResT*>& stateRes, uint& stateCount, IDynamicArray<ResT*>& newRes)
{
	const Span<ResT*> stateSpan(stateRes.GetData(), stateCount);

	if (stateSpan != newRes)
	{
		const uint oldCount = stateCount;
		const uint newCount = (uint)newRes.GetLength();
		const uint updateCount = std::max(oldCount, newCount);
		stateCount = newCount;

		// Write new resources to state cache
		ArrMemCopy(stateRes, newRes);

		// Set extra range to null
		if (oldCount > newCount)
			SetArrNull(stateRes, newCount, oldCount - newCount);

		return updateCount;
	}
	else
		return 0;
}

template<typename ResT, typename MapT, typename DataT>
uint ContextState::TryUpdateResources(IDynamicArray<ResT*>& stateRes, uint& stateCount, const DataT& resSrc, const MapT* pResMap)
{
	if (pResMap != nullptr && pResMap->GetCount() > 0)
	{
		// Map resources into temporary array
		ALLOCA_SPAN(newRes, pResMap->GetCount(), ResT*);
		pResMap->GetResources(resSrc, newRes);

		// Populate the state cache based on the mapped resources
		return TryUpdateResources(stateRes, stateCount, newRes);
	}
	else if (stateCount > 0)
	{
		const uint updateCount = stateCount;
		SetArrNull(stateRes, stateCount);
		stateCount = 0;

		return updateCount;
	}
	else
		return 0;
}

uint ContextState::TryUpdateResources(ShadeStages stage, IDynamicArray<ConstantBuffer>& cbufs)
{
	ContextState::StageState& stageState = GetStage(stage);

	if (cbufs.GetLength() > 0)
	{
		ALLOCA_SPAN(newCbufs, cbufs.GetLength(), ID3D11Buffer*);

		// Upload data to each ConstantBuffer object
		for (uint i = 0; i < cbufs.GetLength(); i++)
			newCbufs[i] = cbufs[i].Get();

		return TryUpdateResources(stageState.cbuffers, stageState.cbufCount, newCbufs);
	}
	else if (stageState.cbufCount > 0)
	{
		const uint updateCount = stageState.cbufCount;
		SetArrNull(stageState.cbuffers, stageState.cbufCount);
		stageState.cbufCount = 0;

		return updateCount;
	}
	else
		return 0;
}

uint ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SamplerList& resSrc, const SamplerMap* pMap)
{
	ContextState::StageState& stageState = GetStage(stage);
	return TryUpdateResources(stageState.samplers, stageState.sampCount, resSrc, pMap);
}

uint ContextState::TryUpdateResources(ShadeStages stage, const ResourceSet::SRVList& resSrc, const ResourceViewMap* pMap)
{
	ContextState::StageState& stageState = GetStage(stage);
	return TryUpdateResources(stageState.srvs, stageState.srvCount, resSrc, pMap);
}

uint ContextState::TryUpdateResources(const ResourceSet::UAVList& resSrc, const UnorderedAccessMap* pMap)
{
	return TryUpdateResources(uavs, uavCount, resSrc, pMap);
}
