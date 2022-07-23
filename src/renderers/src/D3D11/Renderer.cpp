#include "D3D11/Renderer.hpp"
#include <math.h>
#include <chrono>

using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

struct Vertex
{
public:
	vec2 pos;
	vec3 color;
};

Renderer::Renderer(MinWindow* window) :
	WindowComponentBase(window),
	device(), // Create device and context
	swap(*window, device), // Create swap chain for window
	pBackBufView(device.GetRtView(swap.GetBuffer(0))) // Get RT view for swap chain back buf
{ }

void Renderer::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	const float sinOffset = sin(time.count() * .5f),
		cosOffset = cos(time.count() * .5f);
	vec4 color(std::abs(sinOffset), std::abs(cosOffset), std::abs(sinOffset + cosOffset), 1.0f);

	// Clear back buffer to color specified
	device.GetContext()->ClearRenderTargetView(pBackBufView.Get(), reinterpret_cast<float*>(&color));

	UniqueArray<Vertex> vertices = 
	{
		{ { 0.0f, 0.5f }, { color.r, color.g, color.b } },
		{ { 0.5f, -0.5f }, { color.b, color.r, color.g } },
		{ { -0.5f, -0.5f }, { color.g, color.b, color.r  } },
		{ { 0.67f, 0.67f }, { color.b, color.g, color.r } }, // 3
		{ { -0.67f, 0.67f }, { color.b, color.r, color.g } },
	};
	VertexBuffer vBuf(device.Get(), vertices);
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;

	// Assign vertex buffer
	device.GetContext()->IASetVertexBuffers(0u, 1u, vBuf.GetAddressOf(), &stride, &offset);

	// Defines vertex connectivity
	device.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UniqueArray<USHORT> indices = 
	{
		0, 1, 2,
		0, 3, 1,
		2, 4, 0
	};

	// Assign index buffer
	IndexBuffer iBuf(device.Get(), indices);
	device.GetContext()->IASetIndexBuffer(iBuf.Get(), DXGI_FORMAT_R16_UINT, 0);

	ComPtr<ID3DBlob> pBlob;

	// Load VS binary
	GFX_THROW_FAILED(D3DReadFileToBlob(L"DefaultVertShader.cso", &pBlob));

	// Compile and assign VS
	ComPtr<ID3D11VertexShader> pVS;
	GFX_THROW_FAILED(device.Get()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVS));
	device.GetContext()->VSSetShader(pVS.Get(), nullptr, 0u);

	// Define position semantic
	D3D11_INPUT_ELEMENT_DESC vDesc[] = 
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(vec2), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ComPtr<ID3D11InputLayout> pVsLayout;
	device.Get()->CreateInputLayout(
		(D3D11_INPUT_ELEMENT_DESC*)&vDesc,
		(UINT)std::size(vDesc),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&pVsLayout
	);
	device.GetContext()->IASetInputLayout(pVsLayout.Get());

	D3DReadFileToBlob(L"DefaultPixShader.cso", &pBlob);

	// Compile and assign PS
	ComPtr<ID3D11PixelShader> pPS;
	device.Get()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPS);
	device.GetContext()->PSSetShader(pPS.Get(), nullptr, 0u);

	// Set viewport bounds
	D3D11_VIEWPORT viewPort = {};
	viewPort.Width = 640;
	viewPort.Height = 480;
	viewPort.MaxDepth = 1;
	viewPort.MinDepth = 0;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	device.GetContext()->RSSetViewports(1u, &viewPort);

	// Bind back buffer as render target
	device.GetContext()->OMSetRenderTargets(1u, pBackBufView.GetAddressOf(), nullptr);

	device.GetContext()->DrawIndexed((UINT)indices.GetLength(), 0, 0);

	// Present frame
	GFX_THROW_FAILED(swap.Get()->Present(1u, 0));
}
