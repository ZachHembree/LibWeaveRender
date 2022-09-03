#pragma once
#include <dxgi1_2.h>
#include "D3D11/Device.hpp"

namespace Replica
{
	class MinWindow;
}

namespace Replica::D3D11
{
	class RenderTarget;

	class SwapChain : public MoveOnlyObjBase
	{
	public:
		SwapChain(const MinWindow& wnd, Device* dev);

		SwapChain(SwapChain&&) = default;
		SwapChain& operator=(SwapChain&&) = default;

		IDXGISwapChain1* Get() { return pSwap.Get(); }

		IDXGISwapChain1** GetAddressOf() { return pSwap.GetAddressOf(); }

		/// <summary>
		/// Returns interface to swap chain buffer at the given index
		/// </summary>
		RenderTarget GetBuffer(int index);

		/// <summary>
		/// Presents rendered image with the given synchronization settings
		/// </summary>
		void Present(UINT syncInterval, UINT flags);

	private:
		Device* pDev;
		ComPtr<IDXGISwapChain1> pSwap;

	};
}