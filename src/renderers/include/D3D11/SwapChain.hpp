#pragma once
#include <dxgi1_2.h>
#include "D3D11/Device.hpp"

namespace Replica::D3D11
{
	class SwapChain
	{
	public:
		SwapChain(const MinWindow& wnd, Device& dev)
		{
			Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
			GFX_THROW_FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), &dxgiFactory));

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
			fsDesc.RefreshRate.Denominator = 0; // Use current refresh rate
			fsDesc.RefreshRate.Numerator = 0;
			fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Native output, no scaling needed
			fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // No interlacing
			fsDesc.Windowed = TRUE;

			DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
			swapDesc.Width = 0; // Use window size for buffer dimensions
			swapDesc.Height = 0;
			swapDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			swapDesc.Stereo = FALSE;
			swapDesc.SampleDesc.Count = 1; // No MSAA
			swapDesc.SampleDesc.Quality = 0;
			swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapDesc.BufferCount = 2; // Triple buffered
			swapDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // No blending
			swapDesc.Scaling = DXGI_SCALING_NONE; // Native output, no scaling needed
			swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapDesc.Flags = 0;

			GFX_THROW_FAILED(dxgiFactory->CreateSwapChainForHwnd(
				dev.Get(),
				wnd.GetWndHandle(),
				&swapDesc,
				&fsDesc,
				NULL,
				&pSwap
			));
		}

		IDXGISwapChain1* Get() { return pSwap.Get(); }

		IDXGISwapChain1** GetAddressOf() { return pSwap.GetAddressOf(); }

		/// <summary>
		/// Returns interface to swap chain buffer at the given index
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D11Resource> GetBuffer(int index)
		{
			Microsoft::WRL::ComPtr<ID3D11Resource> buf;
			GFX_THROW_FAILED(pSwap->GetBuffer(index, __uuidof(ID3D11Resource), &buf));

			return buf;
		}

		/// <summary>
		/// Presents rendered image with the given synchronization settings
		/// </summary>
		void Present(UINT syncInterval, UINT flags)
		{
			GFX_THROW_FAILED(pSwap->Present(syncInterval, flags));
		}

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwap;

	};
}