#include "RendererD3D11.hpp"
#include <math.h>
#include <chrono>

using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace glm;

RendererD3D11::RendererD3D11(MinWindow* window) :
	WindowComponentBase(window),
	pDevice(nullptr),
	pContext(nullptr),
	pSwapChain(nullptr),
	pBackBufView(nullptr)
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
	swapDesc.BufferCount = 2; // Triple buffered
	swapDesc.OutputWindow = window->GetWndHandle(); // Target window
	swapDesc.Windowed = TRUE;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapDesc.Flags = 0;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr, // Default device
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // No software rasterizer
		D3D11_CREATE_DEVICE_DEBUG, // Enable debugging
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDesc,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	));

	// Retrieve buffer in swap chain at index 0 to get back buf
	ComPtr<ID3D11Resource> backBuf;
	GFX_THROW_FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuf));
	
	// Get back buffer view
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(backBuf.Get(), nullptr, &pBackBufView));
}

void RendererD3D11::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	glm::vec4 color(abs(sin(time.count() * 2)), 0.0f, 0.0f, 1.0f);

	// Clear back buffer to color specified
	pContext->ClearRenderTargetView(pBackBufView.Get(), reinterpret_cast<float*>(&color));

	vec2 vertices[] = 
	{
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
	};

	// Define and create vertex buffer
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = (UINT)(sizeof(vec2) * std::size(vertices));
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vBufDesc;
	vBufDesc.pSysMem = &vertices;
	vBufDesc.SysMemPitch = 0;
	vBufDesc.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> pVertBuf;
	GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, &vBufDesc, &pVertBuf));

	const UINT stride = sizeof(vec2);
	const UINT offset = 0;

	// Assign vertex buffer
	pContext->IASetVertexBuffers(0u, 1u, pVertBuf.GetAddressOf(), &stride, &offset);

	// Defines vertex connectivity
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ComPtr<ID3DBlob> pBlob;

	// Load VS binary
	GFX_THROW_FAILED(D3DReadFileToBlob(L"DefaultVertShader.cso", &pBlob));

	// Compile and assign VS
	ComPtr<ID3D11VertexShader> pVS;
	GFX_THROW_FAILED(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVS));
	pContext->VSSetShader(pVS.Get(), nullptr, 0u);

	// Define position semantic
	D3D11_INPUT_ELEMENT_DESC vDesc = {};
	vDesc.SemanticName = "Position";
	vDesc.SemanticIndex = 0;
	vDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	vDesc.InputSlot = 0;
	vDesc.AlignedByteOffset = 0;
	vDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vDesc.InstanceDataStepRate = 0;

	ComPtr<ID3D11InputLayout> pVsLayout;
	pDevice->CreateInputLayout(
		&vDesc,
		1u,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pVsLayout
	);
	pContext->IASetInputLayout(pVsLayout.Get());

	D3DReadFileToBlob(L"DefaultPixShader.cso", &pBlob);

	// Compile and assign PS
	ComPtr<ID3D11PixelShader> pPS;
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPS);
	pContext->PSSetShader(pPS.Get(), nullptr, 0u);

	// Set viewport bounds
	D3D11_VIEWPORT viewPort = {};
	viewPort.Width = 640;
	viewPort.Height = 480;
	viewPort.MaxDepth = 1;
	viewPort.MinDepth = 0;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	pContext->RSSetViewports(1u, &viewPort);

	// Bind back buffer as render target
	pContext->OMSetRenderTargets(1u, pBackBufView.GetAddressOf(), nullptr);

	pContext->Draw((UINT)std::size(vertices), 0u);
	 
	// Present frame
	GFX_THROW_FAILED(pSwapChain->Present(1u, 0));
}
