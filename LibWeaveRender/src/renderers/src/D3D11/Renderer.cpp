#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/DepthStencilTexture.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/RenderComponent.hpp"

using namespace Weave;
using namespace Weave::D3D11;
using namespace Weave::Effects;

Renderer::Renderer(MinWindow& window) :
	WindowComponentBase(window),
	pDev(new Device(*this)), // Create *pDev and context
	pSwap(new SwapChain(*pDev)), // Create swap chain for window
	pDefaultDS(new DepthStencilTexture()),
	useDefaultDS(true),
	fitToWindow(true),
	pDefaultShaders(new ShaderLibrary(*this, GetBuiltInShaders())),
	frameCount(0),
	canRender(true),
	isFsReq(false),
	isFsAllowed(true),
	lastDispMode(-1)
{ 
	MeshDef quadDef = Primitives::GeneratePlane<VertexPos2D>(ivec2(0), 2.0f);
	defaultMeshes["FSQuad"] = Mesh(*pDev, quadDef);

	defaultSamplers["PointClamp"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::CLAMP);
	defaultSamplers["PointMirror"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::MIRROR);
	defaultSamplers["PointBorder"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::BORDER);

	defaultSamplers["LinearClamp"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::CLAMP);
	defaultSamplers["LinearMirror"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::MIRROR);
	defaultSamplers["LinearBorder"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::BORDER);

	pSwap->SetBufferFormat(Formats::R8G8B8A8_UNORM);
	pSwap->ResizeBuffers(GetWindow().GetMonitorResolution());

	WV_LOG_INFO() << "Renderer Init";
}

Renderer::~Renderer() = default;

Device& Renderer::GetDevice() { return *pDev; }

/*
	General purpose public utilities
*/

IRenderTarget& Renderer::GetBackBuffer() { return pSwap->GetBackBuf(); }

const SwapChain& Renderer::GetSwapChain() const { return *pSwap; }

double Renderer::GetFrameTimeMS() const { return frameTimer.GetElapsedMS(); }

ulong Renderer::GetFrameNumber() const { return frameCount; }

ShaderLibrary Renderer::CreateShaderLibrary(const ShaderLibDef& def) { return ShaderLibrary(*this, def); }

ShaderLibrary Renderer::CreateShaderLibrary(ShaderLibDef&& def) { return ShaderLibrary(*this, std::move(def)); }

ivec2 Renderer::GetOutputResolution() const { return outputRes; }

void Renderer::SetOutputResolution(ivec2 res) 
{ 
	if (!isFsReq)
		outputRes = res; 
}

bool Renderer::GetIsFitToWindow() const { return fitToWindow; }

void Renderer::SetFitToWindow(bool value) { fitToWindow = value; }

bool Renderer::GetIsFullscreen() const { return isFsReq; }

void Renderer::SetFullscreen(bool value) { isFsReq = value; }

uint Renderer::GetActiveDisplay() const { return pSwap->GetDisplayOutput(); }

void Renderer::SetActiveDisplay(uint index) 
{ 
	pSwap->SetDisplayOutput(index); 
	outputRes = pSwap->GetSize();
}

const IDynamicArray<DisplayOutput>& Renderer::GetDisplays() const { return pDev->GetDisplays(); }

Formats Renderer::GetOutputFormat() const { return pSwap->GetBufferFormat(); }

void Renderer::SetOutputFormat(Formats format) { pSwap->SetBufferFormat(format); }

uivec2 Renderer::GetDisplayMode() const { return pSwap->GetDisplayMode(); }

void Renderer::SetDisplayMode(uivec2 newMode) 
{ 
	if (isFsReq && newMode != lastDispMode)
	{
		pSwap->SetDisplayMode(newMode);
		lastDispMode = newMode;
		outputRes = pSwap->GetSize();
	}
}

Viewport Renderer::GetMainViewport() const
{
	return
	{
		pSwap->GetBackBuf().GetRenderOffset(),
		pSwap->GetBackBuf().GetSize(),
		pDefaultDS->GetRange()
	};
}

void Renderer::SetMainViewport(Viewport& vp)
{
	SetOutputResolution(vp.size);
	pSwap->GetBackBuf().SetRenderOffset(vp.offset);
	pDefaultDS->SetRange(vp.zDepth);
}

bool Renderer::GetIsDepthStencilEnabled() const { return useDefaultDS; }

void Renderer::SetIsDepthStencilEnabled(bool value) { useDefaultDS = value; }

/*
	Default resources used for internal functions and generalized samplers
*/

Material& Renderer::GetDefaultMaterial(string_view name) const
{
	const StringIDMap& stringMap = pDefaultShaders->GetStringMap();
	uint stringID;

	D3D_CHECK_MSG(stringMap.TryGetStringID(name, stringID), "Default material undefined");
	const auto& it = defaultMaterials.find(stringID);

	if (it != defaultMaterials.end())
	{
		Material& mat = it->second;
		mat.ResetDefines();
		return mat;
	}
	else
	{
		const auto& pair = defaultMaterials.emplace(stringID, pDefaultShaders->GetMaterial(stringID));
		return pair.first->second;
	}
}

ComputeInstance& Renderer::GetDefaultCompute(string_view name) const
{
	const StringIDMap& stringMap = pDefaultShaders->GetStringMap();
	uint stringID;
	D3D_CHECK_MSG(stringMap.TryGetStringID(name, stringID), "Default compute shader undefined");

	const auto& it = defaultCompute.find(stringID);

	if (it != defaultCompute.end())
		return it->second;
	else
	{
		const auto& pair = defaultCompute.emplace(stringID, pDefaultShaders->GetComputeInstance(stringID));
		return pair.first->second;
	}
}

Mesh& Renderer::GetDefaultMesh(string_view name) const { return const_cast<Mesh&>(defaultMeshes.at(name)); }

Sampler& Renderer::GetDefaultSampler(string_view name) const { return const_cast<Sampler&>(defaultSamplers.at(name)); }

/*
	Core rendering functions
*/

bool Renderer::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
	switch (msg)
	{
	// Monitor Win32 events for occlusion, minimization or resizing that would force 
	// the renderer out of exclusive full screen
	case WM_ACTIVATE:
	{
		const bool isActivating = (wParam != WA_INACTIVE);
		isFsAllowed = isActivating;
		break;
	}
	case WM_ACTIVATEAPP:
	{
		const bool isActivating = (wParam == TRUE);
		isFsAllowed = isActivating;
		break;
	}
	case WM_SIZE:
		if (isFsAllowed && wParam == SIZE_MINIMIZED)
		{
			isFsAllowed = false;
			canRender = false;
		}
		break;
	}

	return true;
}

void Renderer::Update()
{
	CtxImm& ctx = pDev->GetContext();
	const ivec2 bodySize = GetWindow().GetBodySize();

	// Full screen mode validation and transitioning
	const bool isFsEnabled = pSwap->GetIsFullscreen();
	// True if was in exclusive full screen and just occluded
	const bool wasOccludedFS = isFsEnabled && !isFsAllowed;

	if ((isFsReq != pSwap->GetIsFullscreen()) || wasOccludedFS)
	{
		if (!isFsReq || wasOccludedFS) // Disable for occlusion or manual disable	
			pSwap->SetFullscreen(false, wasOccludedFS);
		else if (isFsReq && isFsAllowed)
		{
			pSwap->SetFullscreen(true, false);
			outputRes = pSwap->GetSize();
			lastDispMode = pSwap->GetDisplayMode();
		}
	}

	// Update window scaling
	if (!isFsReq)
	{
		const uivec2 lastBackSize = pSwap->GetSize();

		if (fitToWindow)
			outputRes = bodySize;

		if (outputRes.x > lastBackSize.x || outputRes.y > lastBackSize.y)
			pSwap->ResizeBuffers(outputRes);
	}

	// Deferred swap chain init
	if (!pSwap->GetIsInitialized())
		pSwap->Init();

	pSwap->GetBackBuf().SetRenderSize(outputRes);
	canRender = !isFsReq || isFsAllowed;

	// If rendering is explicitly disabled or resolution invalid, skip everything else
	if (!canRender || bodySize.x == 0 || bodySize.y == 0)
		return;

	BeforeDraw(ctx);

	// Update depth stencil if not overriden
	if (useDefaultDS)
	{
		if (pSwap->GetSize() != pDefaultDS->GetSize())
			*pDefaultDS = DepthStencilTexture(*pDev, pSwap->GetSize());
	}

	// Clear back buffer
	pSwap->GetBackBuf().Clear(ctx);
	
	if (useDefaultDS)
		pDefaultDS->Clear(ctx);

	// Bind back buffer as render target
	if (useDefaultDS)
		ctx.BindRenderTarget(pSwap->GetBackBuf(), *pDefaultDS);
	else
		ctx.BindRenderTarget(pSwap->GetBackBuf());

	DrawEarly(ctx);
	Draw(ctx);
	DrawLate(ctx);

	// Present frame
	ctx.Present(*pSwap, 1, 0);

	AfterDraw(ctx);

	// ImGUI nulls shaders after draw, leaving the state cache out of sync
	ctx.ResetShaders();

	frameCount++;
	frameTimer.Restart();
}

/*
	Render component utilities
*/

bool Renderer::RegisterComponent(RenderComponentBase& component)
{
	if (!component.GetIsRegistered())
	{
		component.pRenderer = this;
		component.isRegistered = true;
		pComponents.Add(&component);
		return true;
	}
	else
		return false;
}

bool Renderer::UnregisterComponent(RenderComponentBase& component)
{
	int index = -1;

	for (int i = (int)pComponents.GetLength() - 1; i >= 0; i--)
	{
		if (pComponents[i] == &component)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		component.isRegistered = false;
		component.pRenderer = nullptr;
		pComponents.RemoveAt(index);
		return true;
	}
	else
		return false;
}

void Renderer::BeforeDraw(CtxImm& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Setup(ctx);
	}
}

void Renderer::DrawEarly(CtxImm& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawEarly(ctx);
	}
}

void Renderer::Draw(CtxImm& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Draw(ctx);
	}
}

void Renderer::DrawLate(CtxImm& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawLate(ctx);
	}	
}

void Renderer::AfterDraw(CtxImm& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->AfterDraw(ctx);
	}
}
