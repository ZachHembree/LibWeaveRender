#pragma once
#include <dxgi1_2.h>
#include "D3D11/Device.hpp"

namespace Replica
{
	class MinWindow;
}

namespace Replica::D3D11
{
	class SwapChain
	{
	public:
		SwapChain(const MinWindow& wnd, Device& dev);

		IDXGISwapChain1* Get() { return pSwap.Get(); }

		IDXGISwapChain1** GetAddressOf() { return pSwap.GetAddressOf(); }

		/// <summary>
		/// Returns interface to swap chain buffer at the given index
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D11Resource> GetBuffer(int index);

		/// <summary>
		/// Presents rendered image with the given synchronization settings
		/// </summary>
		void Present(UINT syncInterval, UINT flags);

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwap;

	};
}