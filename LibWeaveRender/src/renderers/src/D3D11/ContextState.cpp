#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ContextState.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Viewport.hpp"

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
	pShader(nullptr),
	samplers(D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, nullptr),
	cbuffers(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, nullptr),
	resViews(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullptr),
	sampCount(0),
	cbufCount(0),
	srvCount(0)
{}

void ContextState::StageState::Reset()
{
	pShader = nullptr;
	SetArrNull(samplers);
	SetArrNull(cbuffers);
	SetArrNull(resViews);
}

void ContextState::Init()
{
	renderTargets = UniqueArray<ID3D11RenderTargetView*>(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullptr);
	viewports = UniqueArray<Viewport>(D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);
	vertexBuffers = UniqueArray<ID3D11Buffer*>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, nullptr);
	vbStrides = UniqueArray<uint>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, 0);
	vbOffsets = UniqueArray<uint>(D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, 0);
	uavs = UniqueArray<ID3D11UnorderedAccessView*>(D3D11_1_UAV_SLOT_COUNT, nullptr);

	stages = UniqueArray<StageState>(g_ShadeStageCount);
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
