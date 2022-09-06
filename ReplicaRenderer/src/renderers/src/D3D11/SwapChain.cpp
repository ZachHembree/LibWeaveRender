#include <d3d11.h>
#include <wrl.h>
#include "MinWindow.hpp"
#include "D3D11/SwapChain.hpp"
#include "D3D11/Resources/RenderTarget.hpp"
#include "D3D11/Resources/Texture2D.hpp"

using namespace Replica::D3D11;

SwapChain::SwapChain() : pDev(nullptr)
{ }

SwapChain::SwapChain(const MinWindow& wnd, Device& dev) :
	pDev(&dev)
{
	ComPtr<IDXGIFactory2> dxgiFactory;
	GFX_THROW_FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory2), &dxgiFactory));

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.RefreshRate.Denominator = 0; // Use current refresh rate
	fsDesc.RefreshRate.Numerator = 0;
	fsDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Native output, no scaling needed
	fsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // No interlacing
	fsDesc.Windowed = TRUE;

	const ivec2 monRes = wnd.GetMonitorResolution();
	DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
	swapDesc.Width = monRes.x; // Initialize chain to active monitor resolution
	swapDesc.Height = monRes.y;
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
		pDev->Get(),
		wnd.GetWndHandle(),
		&swapDesc,
		&fsDesc,
		NULL,
		&pSwap
	));

	// Depth-Stencil buffer setup
	ID3D11DeviceContext* pCon = pDev->GetContext().Get();
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ComPtr<ID3D11DepthStencilState> pDsState;
	GFX_THROW_FAILED( pDev->Get()->CreateDepthStencilState(&dsDesc, &pDsState));
	pCon->OMSetDepthStencilState(pDsState.Get(), 1);

	depthStencil = Texture2D(
		dev, 
		monRes,
		Formats::D32_FLOAT, 
		ResourceUsages::Default, 
		ResourceTypes::DepthStencil
	);
}

/// <summary>
/// Returns interface to swap chain buffer at the given index
/// </summary>
RenderTarget SwapChain::GetBuffer(int index)
{
	ID3D11Resource* pRes;
	GFX_THROW_FAILED(pSwap->GetBuffer(index, __uuidof(ID3D11Resource), (void**)&pRes));
	
	return RenderTarget(*pDev, pRes, depthStencil.GetDSV());
}

/// <summary>
/// Presents rendered image with the given synchronization settings
/// </summary>
void SwapChain::Present(UINT syncInterval, UINT flags)
{
	GFX_THROW_FAILED(pSwap->Present(syncInterval, flags));
}
