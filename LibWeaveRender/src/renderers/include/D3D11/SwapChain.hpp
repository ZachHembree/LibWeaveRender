#pragma once
#include "Device.hpp"
#include "Resources/Texture2D.hpp"
#include "Resources/RTHandle.hpp"

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
		SwapChain();

		SwapChain(const MinWindow& wnd, Device& dev);

		SwapChain(SwapChain&&) = default;

		SwapChain& operator=(SwapChain&&) = default;

		/// <summary>
		/// Returns pointer to swap chain interface
		/// </summary>
		IDXGISwapChain1& Get();

		/// <summary>
		/// Returns read-only pointer to pointer for swap chain interface
		/// </summary>
		IDXGISwapChain1** const GetAddressOf();

		/// <summary>
		/// Returns pointer to swap chain interface
		/// </summary>
		IDXGISwapChain1* operator->();

		/// <summary>
		/// Returns the dimensions of the chain's buffers
		/// </summary>
		uivec2 GetSize() const;

		/// <summary>
		/// Returns the number of buffers in the chain
		/// </summary>
		uint GetBufferCount() const;

		/// <summary>
		/// Returns the texture format of the swap chain's buffers
		/// </summary>
		Formats GetBufferFormat() const;

		/// <summary>
		/// Returns handle to swap chain back buffer
		/// </summary>
		RTHandle& GetBackBuf();

		/// <summary>
		/// Resizes the swap chain to the given resolution. If optional fields are
		/// left to default, the last set values will be used instead. If new buffer
		/// count is less than previous, existing RTHandles may become invalid.
		/// </summary>
		void ResizeBuffers(
			uivec2 dim,
			uint count = 0,
			Formats format = Formats::UNKNOWN,
			uint flags = 0
		);

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
		void Present(CtxImm& ctx, uint syncInterval, uint flags);

	private:
		ComPtr<IDXGISwapChain1> pSwap;
		DXGI_SWAP_CHAIN_DESC1 desc;
		ComPtr<ID3D11Resource> pBackBuffer;
		ComPtr<ID3D11RenderTargetView> pBackBufRTV;
		RTHandle backBufRt;

		void GetBuffers();
	};
}