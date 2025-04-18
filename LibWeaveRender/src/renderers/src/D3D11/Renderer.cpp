#include "pch.hpp"
#include "WeaveUtils/Win32.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Resources/Sampler.hpp"
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
	pSwap(new SwapChain(window, *pDev)), // Create swap chain for window
	pDefaultDS(new DepthStencilTexture(*pDev, pSwap->GetSize())),
	useDefaultDS(true),
	fitToWindow(true),
	pDefaultShaders(new ShaderLibrary(*this, GetBuiltInShaders()))
{ 
	MeshDef quadDef = Primitives::GeneratePlane<VertexPos2D>(ivec2(0), 2.0f);
	defaultMeshes["FSQuad"] = Mesh(*pDev, quadDef);

	defaultSamplers["PointClamp"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::CLAMP);
	defaultSamplers["PointMirror"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::MIRROR);
	defaultSamplers["PointBorder"] = Sampler(*pDev, TexFilterMode::POINT, TexClampMode::BORDER);

	defaultSamplers["LinearClamp"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::CLAMP);
	defaultSamplers["LinearMirror"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::MIRROR);
	defaultSamplers["LinearBorder"] = Sampler(*pDev, TexFilterMode::LINEAR, TexClampMode::BORDER);
}

Renderer::~Renderer() = default;

/// <summary>
/// Returns the interface to the *pDev the renderer is running on
/// </summary>
Device& Renderer::GetDevice() { return *pDev; }

IRenderTarget& Renderer::GetBackBuffer() { return pSwap->GetBackBuf(); }

/// <summary>
/// Returns reference to the swap chain interface
/// </summary>
SwapChain& Renderer::GetSwapChain() { return *pSwap; }

ShaderLibrary Renderer::CreateShaderLibrary(const ShaderLibDef& def) { return ShaderLibrary(*this, def); }

ShaderLibrary Renderer::CreateShaderLibrary(ShaderLibDef&& def) { return ShaderLibrary(*this, std::move(def)); }

/// <summary>
/// Returns the viewport used with the back buffer
/// </summary>
Viewport Renderer::GetMainViewport() const
{
	return
	{
		pSwap->GetBackBuf().GetRenderOffset(), 
		pSwap->GetBackBuf().GetSize(),
		pDefaultDS->GetRange() 
	};
}

/// <summary>
/// Sets the viewport used with the back buffer
/// </summary>
void Renderer::SetMainViewport(Viewport& vp)
{
	SetOutputResolution(vp.size);
	pSwap->GetBackBuf().SetRenderOffset(vp.offset);
	pDefaultDS->SetRange(vp.zDepth);
}

/// <summary>
/// Returns the rendering resolution used by the renderer
/// </summary>
ivec2 Renderer::GetOutputResolution() const 
{
	return pSwap->GetBackBuf().GetRenderSize();
}

/// <summary>
/// Sets the render resolution to the given value
/// </summary>
void Renderer::SetOutputResolution(ivec2 res)
{
	const ivec2 lastBackSize = pSwap->GetSize(),
		stencilSize = pDefaultDS->GetSize();

	if (res.x > lastBackSize.x || res.y > lastBackSize.y)
		pSwap->ResizeBuffers(res);

	if (res.x > stencilSize.x || res.y > stencilSize.y)
		*pDefaultDS = DepthStencilTexture(*pDev, res);

	pSwap->GetBackBuf().SetRenderSize(res);
}

/// <summary>
/// Returns true if the render resolution is set to match
/// that of the window body being rendered to.
/// </summary>
bool Renderer::GetIsFitToWindow() const { return fitToWindow; }

/// <summary>
/// Set to true if the renderer should automatically match the
/// window resolution.
/// </summary>
void Renderer::SetIsFitToWindow(bool value) { fitToWindow = value; }

/// <summary>
/// Returns true if the default depth stencil buffer is enabled
/// </summary>
bool Renderer::GetIsDepthStencilEnabled() { return useDefaultDS; }

/// <summary>
/// Enable/disable default depth-stencil buffer
/// </summary>
void Renderer::SetIsDepthStencilEnabled(bool value) { useDefaultDS = value; }

/// <summary>
/// Returns reference to a default material
/// </summary>
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

/// <summary>
/// Returns reference to a default compute shader
/// </summary>
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

/// <summary>
/// Retursn a reference to a default mesh
/// </summary>
Mesh& Renderer::GetDefaultMesh(string_view name) const { return const_cast<Mesh&>(defaultMeshes.at(name)); }

/// <summary>
/// Retursn a reference to a default texture sampler
/// </summary>
Sampler& Renderer::GetDefaultSampler(string_view name) const { return const_cast<Sampler&>(defaultSamplers.at(name)); }

bool Renderer::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
	return true;
}

/// <summary>
/// Registers a new render component. Returns false if the component is already registered.
/// </summary>
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

/// <summary>
/// Unregisters the given component from the renderer. Returns false on fail.
/// </summary>
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

void Renderer::Update()
{
	// Reset binds
	Context& ctx = pDev->GetContext();
	const ivec2 bodySize = GetWindow().GetBodySize();

	ctx.Reset();
	// Temporary
	pDev->GetContext().SetPrimitiveTopology(PrimTopology::TRIANGLELIST);

	// Clear back buffer
	pSwap->GetBackBuf().Clear(ctx);

	if (useDefaultDS)
		pDefaultDS->Clear(ctx);

	// Set viewport bounds
	if (fitToWindow)
		SetOutputResolution(bodySize);

	// Bind back buffer as render target
	if (useDefaultDS)
		ctx.BindRenderTarget(pSwap->GetBackBuf(), *pDefaultDS);
	else
		ctx.BindRenderTarget(pSwap->GetBackBuf());

	BeforeDraw(ctx);

	DrawEarly(ctx);
	Draw(ctx);
	DrawLate(ctx);

	// Present frame
	pSwap->Present(1u, 0);

	AfterDraw(ctx);
}

void Renderer::BeforeDraw(Context& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Setup(ctx);
	}
}

void Renderer::DrawEarly(Context& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawEarly(ctx);
	}
}

void Renderer::Draw(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Draw(ctx);
	}
}

void Renderer::DrawLate(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawLate(ctx);
	}	
}

void Renderer::AfterDraw(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->AfterDraw(ctx);
	}
}
