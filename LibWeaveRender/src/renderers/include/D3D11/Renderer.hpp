#pragma once
#include <unordered_map>
#include "WeaveUtils/Utils.hpp"
#include "WeaveUtils/Stopwatch.hpp"
#include "WeaveUtils/WindowComponentBase.hpp"
#include "Viewport.hpp"
#include "ShaderLibrary.hpp"
#include "Resources/DisplayOutput.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "runtimeobject.lib")

namespace Weave::D3D11
{ 
	class Device;
	class SwapChain;

	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;

	class Mesh;
	class Material;
	class ComputeInstance;
	class Sampler;
	class DepthStencilTexture;

	class ShaderLibrary;
	class RenderComponentBase;

	class Renderer : public WindowComponentBase
	{
	public:
		Renderer(MinWindow& window);

		~Renderer();

		/// <summary>
		/// Returns the interface to the device the renderer is running on
		/// </summary>
		Device& GetDevice();

		/// <summary>
		/// Returns a handle to the swap chain's back buffer
		/// </summary>
		IRenderTarget& GetBackBuffer();

		/// <summary>
		/// Returns reference to the swap chain interface
		/// </summary>
		const SwapChain& GetSwapChain() const;

		/// <summary>
		/// Returns the last frame time in milliseconds
		/// </summary>
		double GetFrameTimeMS() const;

		/// <summary>
		/// Returns the current frame number
		/// </summary>
		ulong GetFrameNumber() const;

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
		/// Returns true if the render resolution is set to match that of the window body being 
		/// rendered to. Only applicable if exclusive full screen is disabled.
		/// </summary>
		bool GetIsFitToWindow() const;

		/// <summary>
		/// Set to true if the renderer resolution should match the resolution of the window body.
		/// Combining this mode with the borderless full screen mode of the parent Window will enable
		/// borderless full screen rendering at the desktop resolution. Superceeded by exclusive full
		/// screen.
		/// </summary>
		void SetFitToWindow(bool value);

		/// <summary>
		/// Returns true if exclusive full screen mode has been enabled
		/// </summary>
		bool GetIsFullscreen() const;

		/// <summary>
		/// Enables or disables full screen mode. Disabling full screen mode automatically transitions 
		/// to windowed mode and vice versa. Borderless fullscreen window mode requires disabling this,
		/// enabling fit to window in the renderer, and setting the Window style itself to borderless 
		/// full screen mode.
		/// </summary>
		void SetFullscreen(bool value);

		/// <summary>
		/// Returns the index of the DisplayOutput currently being used as the output
		/// </summary>
		uint GetActiveDisplay() const;

		/// <summary>
		/// Sets the DisplayOutput corresponding to the given index as the render output
		/// </summary>
		void SetActiveDisplay(uint index);

		/// <summary>
		/// Returns a list of available displays. Display modes availble depends on the output format set.
		/// </summary>
		const IDynamicArray<DisplayOutput>& GetDisplays() const;

		/// <summary>
		/// Returns the rendering format used by the swap chain
		/// </summary>
		Formats GetOutputFormat() const;

		/// <summary>
		/// Sets the rendering format used by the swap chain. Changing this will change the display modes 
		/// available.
		/// </summary>
		void SetOutputFormat(Formats format);

		/// <summary>
		/// Returns the ID for the display mode used by the renderer. Results may not be accurate for 
		/// windowed modes. X = DisplayMode index - Y = Refresh rate index
		/// </summary>
		uivec2 GetDisplayMode() const;

		/// <summary>
		/// Sets the display mode corresponding to the given identifier. Only valid in exclusive full screen 
		/// mode.
		/// </summary>
		void SetDisplayMode(uivec2 modeID);

		/// <summary>
		/// Returns true if the default depth stencil buffer is enabled
		/// </summary>
		bool GetIsDepthStencilEnabled() const;

		/// <summary>
		/// Enable/disable default depth-stencil buffer
		/// </summary>
		void SetIsDepthStencilEnabled(bool value);

		/// <summary>
		/// Creates a shader library by copying the given definition
		/// </summary>
		ShaderLibrary CreateShaderLibrary(const ShaderLibDef& def);

		/// <summary>
		/// Creates a shader library by moving the given definition
		/// </summary>
		ShaderLibrary CreateShaderLibrary(ShaderLibDef&& def);

		/// <summary>
		/// Returns reference to a default material
		/// </summary>
		Material& GetDefaultMaterial(string_view name) const;

		/// <summary>
		/// Returns reference to a default compute shader
		/// </summary>
		ComputeInstance& GetDefaultCompute(string_view name) const;

		/// <summary>
		/// Retursn a reference to a default mesh
		/// </summary>
		Mesh& GetDefaultMesh(string_view name) const;

		/// <summary>
		/// Returns reference to a default texture sampler
		/// </summary>
		Sampler& GetDefaultSampler(string_view name) const;

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
		mutable std::unordered_map<uint, ComputeInstance> defaultCompute;
		mutable std::unordered_map<uint, Material> defaultMaterials;
		std::unordered_map<string_view, Mesh> defaultMeshes;
		std::unordered_map<string_view, Sampler> defaultSamplers;

		std::unique_ptr<Device> pDev;
		std::unique_ptr<DepthStencilTexture> pDefaultDS;

		std::unique_ptr<SwapChain> pSwap;
		uivec2 outputRes;
		uivec2 lastDispMode;
		bool fitToWindow;
		bool isFsReq;

		std::unique_ptr<ShaderLibrary> pDefaultShaders;
		UniqueVector<RenderComponentBase*> pComponents;

		bool useDefaultDS;
		bool canRender;
		bool isFsAllowed;

		ulong frameCount;
		Stopwatch frameTimer;

		void BeforeDraw(CtxImm& ctx);

		void DrawEarly(CtxImm& ctx);

		void Draw(CtxImm& ctx);

		void DrawLate(CtxImm& ctx);

		void AfterDraw(CtxImm& ctx);
	};
}