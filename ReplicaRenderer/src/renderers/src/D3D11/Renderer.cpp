#include "MinWindow.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Resources/Texture2D.hpp"

using namespace Replica::D3D11;

Renderer::Renderer(MinWindow& window) :
	WindowComponentBase(window),
	device(), // Create device and context
	swap(window, device), // Create swap chain for window
	backBuf(swap.GetBuffer(0)) // Get RT view for swap chain back buf
{ }

void Renderer::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ }

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

	for (int i = pComponents.GetLength() - 1; i >= 0; i--)
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
	ctx.Reset();

	// Clear back buffer
	backBuf.Clear(ctx);

	BeforeDraw(ctx);

	// Set viewport bounds
	ctx.RSSetViewport(GetParent().GetBodySize());
	// Bind back buffer as render target
	backBuf.Bind(ctx);

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
