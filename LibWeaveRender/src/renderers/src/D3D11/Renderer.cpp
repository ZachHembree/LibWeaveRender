#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "FrameTimer.hpp"
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

Renderer::Renderer(MinWindow& parent) :
	WindowComponentBase(parent, 10),
	pDev(new Device(*this)), // Create *pDev and context
	pSwap(new SwapChain(*pDev)), // Create swap chain for window
	pDefaultDS(new DepthStencilTexture()),
	fsMode(WindowRenderModes::Windowed),
	outputRes(GetWindow().GetMonitorResolution()),
	lastDispMode(-1),
	pDefaultShaders(new ShaderLibrary(*this, GetBuiltInShaders())),
	useDefaultDS(true),
	canRender(true),
	isFsAllowed(true),
	pFrameTimer(new FrameTimer()),
	targetFPS(0)
{ 
	MeshDef quadDef = Primitives::GeneratePlane<VertexPos2D>(ivec2(0), 2.0f);
	defaultMeshes["FSQuad"] = Mesh(*pDev, quadDef);

	defaultSamplers["PointClamp"] = Sampler(*pDev, TexFilterModes::POINT, TexClampModes::CLAMP);
	defaultSamplers["PointMirror"] = Sampler(*pDev, TexFilterModes::POINT, TexClampModes::MIRROR);
	defaultSamplers["PointBorder"] = Sampler(*pDev, TexFilterModes::POINT, TexClampModes::BORDER);

	defaultSamplers["LinearClamp"] = Sampler(*pDev, TexFilterModes::LINEAR, TexClampModes::CLAMP);
	defaultSamplers["LinearMirror"] = Sampler(*pDev, TexFilterModes::LINEAR, TexClampModes::MIRROR);
	defaultSamplers["LinearBorder"] = Sampler(*pDev, TexFilterModes::LINEAR, TexClampModes::BORDER);

	pSwap->SetBufferFormat(Formats::R8G8B8A8_UNORM);
	pSwap->ResizeBuffers(outputRes);
	pFrameTimer->SetTargetFrameTimeNS(GetTimeHZtoNS(180));

	WV_LOG_INFO() << "Renderer Init";
}

Renderer::~Renderer() = default;

Device& Renderer::GetDevice() { return *pDev; }

/*
	General purpose public utilities
*/

IRenderTarget& Renderer::GetBackBuffer() { return pSwap->GetBackBuf(); }

ulong Renderer::GetFrameNumber() const { return pFrameTimer->GetFrameCount(); }

ShaderLibrary Renderer::CreateShaderLibrary(const ShaderLibDef& def) { return ShaderLibrary(*this, def); }

ShaderLibrary Renderer::CreateShaderLibrary(ShaderLibDef&& def) { return ShaderLibrary(*this, std::move(def)); }

uivec2 Renderer::GetOutputResolution() const { return outputRes; }

void Renderer::SetOutputResolution(ivec2 res) 
{ 
	if (fsMode != WindowRenderModes::ExclusiveFS)
		outputRes = res; 
}

WindowRenderModes Renderer::GetWindowRenderMode() const { return fsMode; }

void Renderer::SetWindowRenderMode(WindowRenderModes mode) { fsMode = mode; }

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
	if (newMode != lastDispMode)
	{
		pSwap->SetDisplayMode(newMode);
		lastDispMode = newMode;
		outputRes = pSwap->GetSize();
	}
}

bool Renderer::GetIsSyncModeSupported(VSyncRenderModes mode) const { return pSwap->GetIsSyncModeSupported(mode); }

VSyncRenderModes Renderer::GetSyncMode() const { return pSwap->GetSyncMode(); }

bool Renderer::TrySetSyncMode(VSyncRenderModes mode) { return pSwap->TrySetSyncMode(mode); }

double Renderer::GetFrameRateAvgFPS() const { return GetTimeNStoHZ(pFrameTimer->GetAverageFrameTimeNS()); }

double Renderer::GetTargetFrameRateFPS() const { return targetFPS; }

void Renderer::SetTargetFrameRateFPS(double fps) { targetFPS = fps; }

Viewport Renderer::GetMainViewport() const
{
	return
	{
		pSwap->GetBackBuf().GetRenderOffset(),
		pSwap->GetBackBuf().GetSize(),
		pDefaultDS->GetRange()
	};
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

bool Renderer::OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam)
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

void Renderer::UpdateSwap()
{
	const bool isBorderless = fsMode == WindowRenderModes::BorderlessFS;
	const bool isExclusiveSet = fsMode == WindowRenderModes::ExclusiveFS;
	const bool isFitToWindow = fsMode == WindowRenderModes::Windowed || isBorderless;

	// Full screen mode validation and transitioning
	const bool isFsEnabled = pSwap->GetIsFullscreen();
	// True if was in exclusive full screen and just occluded
	const bool wasOccludedFS = isFsEnabled && !isFsAllowed;

	if ((isExclusiveSet != pSwap->GetIsFullscreen()) || wasOccludedFS)
	{
		if (!isExclusiveSet || wasOccludedFS) // Disable for occlusion or manual disable	
			pSwap->SetFullscreen(false, wasOccludedFS);
		else if (isExclusiveSet && isFsAllowed)
		{
			pSwap->SetFullscreen(true, false);
			outputRes = pSwap->GetSize();
			lastDispMode = pSwap->GetDisplayMode();
		}
	}

	// Update window scaling
	if (!isExclusiveSet)
	{
		// Update window style and size for borderless mode
		GetWindow().SetFullScreen(isBorderless);

		const uivec2 lastBackSize = pSwap->GetSize();

		if (isFitToWindow)
			outputRes = GetWindow().GetBodySize();

		if (AllTrue(outputRes.load() > lastBackSize))
			pSwap->ResizeBuffers(outputRes);
	}

	// Deferred swap chain init
	if (!pSwap->GetIsInitialized())
		pSwap->Init();

	pSwap->GetBackBuf().SetRenderSize(outputRes);
	canRender = (!isExclusiveSet || isFsAllowed) && AllTrue(outputRes.load() > uivec2(0));

	// Update refresh cycle time for frame limiter
	const uivec2 refreshRate = pSwap->GetRefresh();
	const double refreshHz = (double)refreshRate.x / (double)refreshRate.y;
	pFrameTimer->SetNativeRefreshCycleNS(GetTimeHZtoNS(refreshHz));

	if (pSwap->GetSyncMode() == VSyncRenderModes::VariableRefresh)
	{
		if (targetFPS > 1.0 && targetFPS < refreshHz)
			pFrameTimer->SetTargetFrameTimeNS(GetTimeHZtoNS(targetFPS));
		else
			pFrameTimer->SetTargetFrameTimeNS(GetTimeHZtoNS(refreshHz - 2.0));
	}
	else
		pFrameTimer->SetTargetFrameTimeNS(GetTimeHZtoNS(targetFPS));
}

void Renderer::Update()
{
	// Mark frame start
	pFrameTimer->BeginPresent();
	UpdateSwap();

	// If rendering is explicitly disabled, skip everything else
	if (!canRender)
		return;

	CtxImm& ctx = pDev->GetContext();

	// Custom setup
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

	Draw(ctx);

	// Present frame
	const bool canSync = pSwap->GetSyncMode() == VSyncRenderModes::TripleBuffered;
	pSwap->Present(pFrameTimer->WaitPresent(canSync));
	ctx.EndFrame();

	AfterDraw(ctx);
	
	// Mark frame end
	pFrameTimer->EndPresent();
}

/*
	Render component utilities
*/

void Renderer::BeforeDraw(CtxImm& ctx)
{ 
	CompSpan comps = GetComponents();

	for (RenderComponentBase* pComp : comps)
	{
		pComp->Setup(ctx);
	}
}

void Renderer::Draw(CtxImm& ctx)
{
	CompSpan comps = GetComponents();

	for (RenderComponentBase* pComp : comps)
	{
		pComp->Draw(ctx);
	}
}

void Renderer::AfterDraw(CtxImm& ctx)
{
	CompSpan comps = GetComponents();

	for (RenderComponentBase* pComp : comps)
	{
		pComp->AfterDraw(ctx);
	}
}
