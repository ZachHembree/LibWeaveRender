#pragma once
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <unordered_map>
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/WindowComponentBase.hpp"
#include "Viewport.hpp"
#include "ShaderLibrary.hpp"
#include "Resources/DisplayOutput.hpp"
#include "RenderModes.hpp"

namespace Weave
{
	class FrameTimer;
}

namespace Weave::D3D11
{ 
	class Device;
	class SwapChain;
	class CtxImm;
	class CtxBase;

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

	/// <summary>
	/// Unique pointer containing a render component
	/// </summary>
	using RenderCompHandle = std::unique_ptr<RenderComponentBase>;

	/// <summary>
	/// D3D11 renderer main class extending WindowComponentBase. Lifetime managed by a MinWindow instance. 
	/// Rendering and render component updates occur on a separate thread.
	/// </summary>
	class Renderer : public WindowComponentBase, public ComponentManagerBase<Renderer, RenderComponentBase>
	{
	public:
		Renderer(MinWindow& parent);

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
		/// Returns the current frame number
		/// </summary>
		ulong GetFrameNumber() const;

		/// <summary>
		/// Returns the viewport used with the back buffer
		/// </summary>
		Viewport GetMainViewport() const;

		/// <summary>
		/// Returns the resolution of the back buffer
		/// </summary>
		uivec2 GetOutputResolution() const;

		/// <summary>
		/// Sets the output resolution to the given value
		/// </summary>
		void SetOutputResolution(ivec2 res);

		/// <summary>
		/// Returns an enum indciating whether the renderer is in windowed, borderless or exclusive full
		/// screen mode.
		/// </summary>
		WindowRenderModes GetWindowRenderMode() const;

		/// <summary>
		/// Sets windowed, borderless or exclusive full screen mode
		/// </summary>
		void SetWindowRenderMode(WindowRenderModes mode);

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
		/// Returns true if the given sync mode can be set in the current configuration
		/// </summary>
		bool GetIsSyncModeSupported(VSyncRenderModes mode) const;

		/// <summary>
		/// Returns the current vsync mode
		/// </summary>
		VSyncRenderModes GetSyncMode() const;

		/// <summary>
		/// Attempts to configure the given sync mode and returns true on success
		/// </summary>
		bool TrySetSyncMode(VSyncRenderModes mode);

		/// <summary>
		/// Returns the average frame rate in frames per second
		/// </summary>
		double GetFrameRateAvgFPS() const;

		/// <summary>
		/// Returns the frame rate limit in frames per second. If less this value is less than 1, the limit 
		/// is disabled.
		/// If VSync is enabled, it will apply if the target frame rate is an integer multiple of the
		/// refresh rate.
		/// If variable refresh is enabled, the maximum refresh rate will be used as the limit unless a lower
		/// value is specified here.
		/// </summary>
		double GetTargetFrameRateFPS() const;

		/// <summary>
		/// Sets the target frame rate limit in frames per second. If less this value is less than 1, the limit 
		/// is disabled.
		/// If VSync is enabled, it will apply if the target frame rate is an integer multiple of the
		/// refresh rate.
		/// If variable refresh is enabled, the maximum refresh rate will be used as the limit unless a lower
		/// value is specified here.
		/// </summary>
		void SetTargetFrameRateFPS(double fps);

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
		/// Returns a reference to a default mesh
		/// </summary>
		Mesh& GetDefaultMesh(string_view name) const;

		/// <summary>
		/// Returns reference to a default texture sampler
		/// </summary>
		Sampler& GetDefaultSampler(string_view name) const;

	private:
		std::jthread renderThread;
		std::atomic<bool> canRun;
		std::atomic<bool> isPaused;
		std::mutex renderMutex;

		mutable std::unordered_map<uint, ComputeInstance> defaultCompute;
		mutable std::unordered_map<uint, Material> defaultMaterials;
		std::unordered_map<string_view, Mesh> defaultMeshes;
		std::unordered_map<string_view, Sampler> defaultSamplers;

		std::unique_ptr<Device> pDev;
		std::unique_ptr<SwapChain> pSwap;
		std::unique_ptr<DepthStencilTexture> pDefaultDS;
		std::atomic<WindowRenderModes> fsMode;
		std::atomic<uivec2> outputRes;
		uivec2 lastDispMode;

		std::unique_ptr<ShaderLibrary> pDefaultShaders;

		std::atomic<double> targetFPS;
		std::atomic<bool> useDefaultDS;
		std::atomic<bool> canRender;
		std::atomic<bool> isFsAllowed;

		std::unique_ptr<FrameTimer> pFrameTimer;

		/// <summary>
		/// Updates the state of the renderer
		/// </summary>
		void RenderUpdate();

		/// <summary>
		/// Handles event messages from the Win32 API
		/// </summary>
		bool OnWndMessage(HWND hWnd, uint msg, ulong wParam, slong lParam) override;

		void UpdateSwap();

		void BeforeDraw(CtxImm& ctx);

		void Draw(CtxImm& ctx);

		void AfterDraw(CtxImm& ctx);
	};
}