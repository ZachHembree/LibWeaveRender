#include "RendererD3D11.hpp"
#include <math.h>
#include <chrono>

using namespace std::chrono;

RendererD3D11::RendererD3D11(MinWindow* window) :
	WindowComponentBase(window),
	d3dDevice(nullptr),
	devContext(nullptr),
	swapChain(nullptr),
	backBufRT(nullptr)
{
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferDesc.Width = 0; // Use window size for buffer dimensions
	swapDesc.BufferDesc.Height = 0;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 0; // Use current refresh rate
	swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Native output, no scaling needed
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // No interlacing
	swapDesc.SampleDesc.Count = 1; // No MSAA
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1; // Double buffered
	swapDesc.OutputWindow = window->GetWndHandle(); // Target window
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel;

	D3D11CreateDeviceAndSwapChain(
		nullptr, // Default device
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // No software rasterizer
		0, // No Flags
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&d3dDevice,
		&featureLevel,
		&devContext
	);

	// Retrieve buffer in swap chain at index 0 to get back buf
	ID3D11Resource* backBuf = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuf));

	// Get back buffer RT
	d3dDevice->CreateRenderTargetView(backBuf, nullptr, &backBufRT);

	// Back buffer handle no longer requried once an RT handle is obtained
	backBuf->Release();
}

RendererD3D11::~RendererD3D11()
{
	// Release D3D COM objects
	if (backBufRT != nullptr)
		backBufRT->Release();

	if (devContext != nullptr)
		devContext->Release();

	if (swapChain != nullptr)
		swapChain->Release();

	if (d3dDevice != nullptr)
		d3dDevice->Release();
}

void RendererD3D11::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	const glm::vec4 color(abs(sin(time.count() * 2)), 0.0f, 0.0f, 1.0f);

	// Clear back buffer to color specified
	devContext->ClearRenderTargetView(backBufRT, reinterpret_cast<const float*>(&color));

	// Present frame
	swapChain->Present(1u, 0);
}
