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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "runtimeobject.lib")

using namespace Weave;
using namespace Weave::D3D11;
using namespace Weave::Effects;

Renderer::Renderer(MinWindow& parent) :
	WindowComponentBase(parent, WndCompPriorities::PaintUpdate),
	pDev(new Device(*this)), // Create *pDev and context
	pSwap(new SwapChain(*pDev)), // Create swap chain for window
	pDefaultDS(new DepthStencilTexture()),
	fsMode(WindowRenderModes::Windowed),
	outputRes(GetWindow().GetMonitorResolution()),
	lastDispMode(-1),
	useDefaultDS(true),
	canRender(true),
	canRun(false),
	isPaused(false),
	isFsAllowed(true),
	pFrameTimer(new FrameTimer()),
	targetFPS(0)
{ 
	RegisterShaderLibrary(GetBuiltInShaders());

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

	renderThread = std::jthread([this] 
	{
		WV_LOG_INFO() << "Renderer Thread Init";
		canRun = true;

		while (canRun)
		{
			std::unique_lock lock(renderMutex);
			RenderUpdate();
		}
	});
}

Renderer::~Renderer()
{
	canRun = false;
	std::unique_lock lock(renderMutex);
}

/*
	General purpose public utilities
*/

Device& Renderer::GetDevice() { return *pDev; }

IRenderTarget& Renderer::GetBackBuffer() { return pSwap->GetBackBuf(); }

const Device& Renderer::GetDevice() const { return *pDev; }

const IRenderTarget& Renderer::GetBackBuffer() const { return pSwap->GetBackBuf(); }

ulong Renderer::GetFrameNumber() const { return pFrameTimer->GetFrameCount(); }

const ShaderLibrary& Renderer::RegisterShaderLibrary(const ShaderLibDef::Handle& def) 
{ 
	const ShaderLibrary& lib = shaderLibs.EmplaceBack(ShaderLibrary(*this, def));

	D3D_CHECK_MSG(shaderLibNameMap.find(lib.GetName()) == shaderLibNameMap.end(), "Shader library names must be unique.");
	shaderLibNameMap.emplace(lib.GetName(), (uint)(shaderLibs.GetLength() - 1));

	return lib;
}

const ShaderLibrary& Renderer::RegisterShaderLibrary(ShaderLibDef&& def) 
{ 
	const ShaderLibrary& lib = shaderLibs.EmplaceBack(ShaderLibrary(*this, std::move(def)));

	D3D_CHECK_MSG(shaderLibNameMap.find(lib.GetName()) == shaderLibNameMap.end(), "Shader library names must be unique.");
	shaderLibNameMap.emplace(lib.GetName(), (uint)(shaderLibs.GetLength() - 1));

	return lib;
}

const ShaderLibrary& Renderer::GetShaderLibrary(string_view name) const
{
	return GetShaderLibrary(GetShaderLibraryID(name));
}

const ShaderLibrary& Renderer::GetShaderLibrary(uint id) const
{
	D3D_CHECK_MSG((id < shaderLibs.GetLength() && shaderLibs[id].GetIsValid()),
		"Shader Libary ID ({}) is invalid.", id);

	return shaderLibs[id];
}

uint Renderer::GetShaderLibraryID(string_view name) const
{
	const auto& it = shaderLibNameMap.find(name);
	D3D_CHECK_MSG((!name.empty() && it != shaderLibNameMap.end() && shaderLibs[it->second].GetIsValid()),
		"Shader Libary Name ({}) is invalid.", name);

	return it->second;
}

Material Renderer::GetMaterial(uint libID, uint effectID) const
{
	return GetShaderLibrary(libID).GetMaterial(effectID);
}

ComputeInstance Renderer::GetComputeInstance(uint libID, uint shaderID) const
{
	return GetShaderLibrary(libID).GetComputeInstance(shaderID);
}

Material Renderer::GetMaterial(string_view libName, string_view effectName) const
{
	return GetShaderLibrary(libName).GetMaterial(effectName);
}

ComputeInstance Renderer::GetComputeInstance(string_view libName, string_view name) const
{
	return GetShaderLibrary(libName).GetComputeInstance(name);
}

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
	const StringIDMap& stringMap = shaderLibs[0].GetStringMap();
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
		const auto& pair = defaultMaterials.emplace(stringID, shaderLibs[0].GetMaterial(stringID));
		return pair.first->second;
	}
}

ComputeInstance& Renderer::GetDefaultCompute(string_view name) const
{
	const StringIDMap& stringMap = shaderLibs[0].GetStringMap();
	uint stringID;
	D3D_CHECK_MSG(stringMap.TryGetStringID(name, stringID), "Default compute shader undefined");

	const auto& it = defaultCompute.find(stringID);

	if (it != defaultCompute.end())
		return it->second;
	else
	{
		const auto& pair = defaultCompute.emplace(stringID, shaderLibs[0].GetComputeInstance(stringID));
		return pair.first->second;
	}
}

Mesh& Renderer::GetDefaultMesh(string_view name) const { return const_cast<Mesh&>(defaultMeshes.at(name)); }

const Sampler& Renderer::GetDefaultSampler(string_view name) const { return const_cast<const Sampler&>(defaultSamplers.at(name)); }

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
		isPaused = wParam == SIZE_MINIMIZED;

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

void Renderer::RenderUpdate()
{
	// Mark frame start
	pFrameTimer->BeginPresent();
	UpdateSwap();

	// If rendering is explicitly disabled, skip everything else
	if (isPaused || !canRender)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		return;
	}

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
	pSwap->Present(pFrameTimer->WaitPresent(canSync, slong(2E5)));
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
