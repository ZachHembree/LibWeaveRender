#pragma once
#include "Device.hpp"
#include "Resources/Texture2D.hpp"
#include "Resources/RTHandle.hpp"
#include "RenderModes.hpp"

namespace Weave
{
	class MinWindow;
}

namespace Weave::D3D11
{
	class RTHandle;

	class SwapChain : public DeviceChild
	{
	public:
		SwapChain(Device& dev);

		SwapChain(SwapChain&&) = default;

		SwapChain& operator=(SwapChain&&) = default;

		/// <summary>
		/// Initializes swap chain after initial renderer config
		/// </summary>
		void Init();

		/// <summary>
		/// Returns true if the swap chain has been configured and initialzed
		/// </summary>
		bool GetIsInitialized() const;

		/// <summary>
		/// Returns the dimensions of the chain's buffers
		/// </summary>
		uivec2 GetSize() const;

		/// <summary>
		/// Returns the current refresh as a rational value.
		/// </summary>
		uivec2 GetRefresh() const;

		/// <summary>
		/// Returns the number of buffers in the chain
		/// </summary>
		uint GetBufferCount() const;

		/// <summary>
		/// Sets the buffer format used for the swap chain and updates available display modes
		/// </summary>
		void SetBufferFormat(Formats format);

		/// <summary>
		/// Returns the texture format of the swap chain's buffers
		/// </summary>
		Formats GetBufferFormat() const;

		/// <summary>
		/// Returns handle to swap chain back buffer
		/// </summary>
		RTHandle& GetBackBuf();

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
		/// Resizes the swap chain to the given resolution.
		/// </summary>
		void ResizeBuffers(uivec2 dim);

		/// <summary>
		/// Returns the index of the DisplayOutput currently being rendered to by the swap chain.
		/// </summary>
		uint GetDisplayOutput() const;

		/// <summary>
		/// Sets the swap chain output to the display corresponding to the given index
		/// </summary>
		void SetDisplayOutput(uint index);

		/// <summary>
		/// Sets the display mode corresponding to the given identifier. In exclusive full screen, this 
		/// will set refresh rate and resolution. In windowed modes, this only resizes buffers.
		/// </summary>
		void SetDisplayMode(uivec2 modeID);

		/// <summary>
		/// Returns the display mode identifier that most closely aligns with the current resolution and 
		/// refresh rate.
		/// </summary>
		uivec2 GetDisplayMode();

		/// <summary>
		/// Returns true if exclusive full screen is enabled
		/// </summary>
		bool GetIsFullscreen() const;

		/// <summary>
		/// Enables or disables exclusive full screen mode. isOccluded should be set when transitioning 
		/// out of exclusive full screen mode into a minimized or occluded state.
		/// </summary>
		void SetFullscreen(bool isFullscreen, bool isOccluded);

		/// <summary>
		/// Presents rendered image with the given synchronization settings
		/// </summary>
		void Present();

	private:
		ComPtr<IDXGIFactory5> pFactory;
		ComPtr<IDXGISwapChain4> pSwap;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc;
		DXGI_SWAP_CHAIN_DESC1 desc;
		bool isInitialized;
		bool isTearingSupported;

		ComPtr<ID3D11Resource> pBackBuffer;
		ComPtr<ID3D11RenderTargetView> pBackBufRTV;
		RTHandle backBufRt;
		VSyncRenderModes syncMode;

		void ApplyDesc();
	};
}