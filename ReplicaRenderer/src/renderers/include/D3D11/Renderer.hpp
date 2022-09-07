#pragma once
#include "WindowComponentBase.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/RenderComponent.hpp"
#include "D3D11/Resources/RenderTarget.hpp"
#include "D3D11/Resources/DepthStencilTexture.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "runtimeobject.lib")

namespace Replica::D3D11
{ 
	class Device;
	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class SwapChain;

	class Renderer : public WindowComponentBase
	{
	public:
		Renderer(MinWindow& window);

		/// <summary>
		/// Returns the interface to the device the renderer is running on
		/// </summary>
		Device& GetDevice() { return device; }

		/// <summary>
		/// Updates the state of the renderer
		/// </summary>
		void Update() override;

		/// <summary>
		/// Handles event messages from the Win32 API
		/// </summary>
		bool OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

		/// <summary>
		/// Registers a new render component. Returns false if the component is already registered.
		/// </summary>
		bool RegisterComponent(RenderComponentBase& component);

		/// <summary>
		/// Unregisters the given component from the renderer. Returns false on fail.
		/// </summary>
		bool UnregisterComponent(RenderComponentBase& component);

	private:
		Device device;
		SwapChain swap;
		RTHandle& backBuf;
		DepthStencilTexture defaultDS;
		UniqueVector<RenderComponentBase*> pComponents;

		void BeforeDraw(Context& ctx);

		void DrawEarly(Context& ctx);

		void Draw(Context& ctx);

		void DrawLate(Context& ctx);

		void AfterDraw(Context& ctx);
	};
}