#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"

using namespace Replica::D3D11;

Renderer::Renderer(MinWindow& window) :
	WindowComponentBase(window),
	device(), // Create device and context
	swap(window, device), // Create swap chain for window
	defaultDS(device, swap.GetSize()),
	useDefaultDS(true)
{ }

/// <summary>
/// Returns the interface to the device the renderer is running on
/// </summary>
Device& Renderer::GetDevice() { return device; }

/// <summary>
/// Returns reference to the swap chain interface
/// </summary>
SwapChain& Renderer::GetSwapChain() { return swap; }

/// <summary>
/// Returns true if the default depth stencil buffer is enabled
/// </summary>
bool Renderer::GetIsDepthStencilEnabled() { return useDefaultDS; }

/// <summary>
/// Enable/disable default depth-stencil buffer
/// </summary>
void Renderer::SetIsDepthStencilEnabled(bool value) { useDefaultDS = value; }

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
		pComponents.push_back(&component);
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
	Context& ctx = device.GetContext();
	const ivec2 bodySize = GetWindow().GetBodySize(),
		lastBackSize = swap.GetSize(),
		stencilSize = defaultDS.GetSize();

	ctx.Reset();

	if (bodySize.x > lastBackSize.x || bodySize.y > lastBackSize.y)
		swap.ResizeBuffers(bodySize);

	if (bodySize.x > stencilSize.x || bodySize.y > stencilSize.y)
		defaultDS = DepthStencilTexture(device, bodySize);

	// Clear back buffer
	swap.GetBackBuf().Clear(ctx);

	if (useDefaultDS)
		defaultDS.Clear(ctx);

	// Set viewport bounds
	ctx.RSSetViewport(bodySize);

	// Bind back buffer as render target
	if (useDefaultDS)
		ctx.SetRenderTarget(swap.GetBackBuf(), defaultDS);
	else
		ctx.SetRenderTarget(swap.GetBackBuf());

	BeforeDraw(ctx);

	DrawEarly(ctx);
	Draw(ctx);
	DrawLate(ctx);

	// Present frame
	swap.Present(1u, 0);

	AfterDraw(ctx);
}

void Renderer::BeforeDraw(Context& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Update(ctx);
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
