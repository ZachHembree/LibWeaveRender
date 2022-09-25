#pragma once
#include <unordered_map>

#include <string_view>
#include "../src/utils/include/WindowComponentBase.hpp"
#include "SwapChain.hpp"
#include "Device.hpp"
#include "RenderComponent.hpp"
#include "Resources/DepthStencilTexture.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "runtimeobject.lib")

namespace Replica::D3D11
{ 
	using std::unordered_map;

	class Device;
	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class SwapChain;
	class Sampler;

	class Renderer : public WindowComponentBase
	{
	public:
		Renderer(MinWindow& window);

		/// <summary>
		/// Returns the interface to the device the renderer is running on
		/// </summary>
		Device& GetDevice();

		/// <summary>
		/// Returns reference to the swap chain interface
		/// </summary>
		SwapChain& GetSwapChain();

		/// <summary>
		/// Returns the viewport used with the back buffer
		/// </summary>
		Viewport GetMainViewport() const;

		/// <summary>
		/// Sets the viewport used with the back buffer
		/// </summary>
		void SetMainViewport(Viewport& vp);

		/// <summary>
		/// Returns the resolution of the back buffer
		/// </summary>
		ivec2 GetOutputResolution() const;

		/// <summary>
		/// Sets the output resolution to the given value
		/// </summary>
		void SetOutputResolution(ivec2 res);

		/// <summary>
		/// Returns true if the render resolution is set to match
		/// that of the window body being rendered to.
		/// </summary>
		bool GetIsFitToWindow() const;

		/// <summary>
		/// Set to true if the renderer should automatically match the
		/// window resolution.
		/// </summary>
		void SetIsFitToWindow(bool value);

		/// <summary>
		/// Returns true if the default depth stencil buffer is enabled
		/// </summary>
		bool GetIsDepthStencilEnabled();

		/// <summary>
		/// Enable/disable default depth-stencil buffer
		/// </summary>
		void SetIsDepthStencilEnabled(bool value);

		/// <summary>
		/// Returns reference to a default effect
		/// </summary>
		Effect& GetDefaultEffect(wstring_view name) const;

		/// <summary>
		/// Returns reference to a default compute shader
		/// </summary>
		ComputeShader& GetDefaultCompute(wstring_view name) const;

		/// <summary>
		/// Retursn a reference to a default mesh
		/// </summary>
		Mesh& GetDefaultMesh(wstring_view name) const;

		/// <summary>
		/// Returns reference to a default texture sampler
		/// </summary>
		Sampler& GetDefaultSampler(wstring_view name) const;

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
		unordered_map<wstring_view, Effect> defaultEffects;
		unordered_map<wstring_view, ComputeShader> defaultCompute;
		unordered_map<wstring_view, Mesh> defaultMeshes;
		unordered_map<wstring_view, Sampler> defaultSamplers;

		UniqueVector<RenderComponentBase*> pComponents;
		Device device;
		mutable SwapChain swap;
		DepthStencilTexture defaultDS;
		bool fitToWindow;
		bool useDefaultDS;

		void BeforeDraw(Context& ctx);

		void DrawEarly(Context& ctx);

		void Draw(Context& ctx);

		void DrawLate(Context& ctx);

		void AfterDraw(Context& ctx);
	};
}