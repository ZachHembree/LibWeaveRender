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
	pBackBufView(device.GetRtView(swap.GetBuffer(0))), // Get RT view for swap chain back buf
	vBuf(device, UniqueArray<Vertex>{
		{ { 0.0f, 0.5f }, { 1.0, 0.0, 0.0 } },
		{ { 0.5f, -0.5f }, { 0.0, 1.0, 0.0 } },
		{ { -0.5f, -0.5f }, { 0.0, 0.0, 1.0  } },
		{ { 0.67f, 0.67f }, { 0.0, 1.0, 0.0 } }, // 3
		{ { -0.67f, 0.67f }, { 1.0, 0.0, 0.0 } },
		}),
	iBuf(device, UniqueArray<USHORT>{
		0, 1, 2,
		0, 3, 1,
		2, 4, 0
		})
{ 
	
}

void Renderer::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	const float sinOffset = sin(time.count() * .5f),
		cosOffset = cos(time.count() * .5f);
	vec4 color(std::abs(sinOffset), std::abs(cosOffset), std::abs(sinOffset + cosOffset), 1.0f);

	// Clear back buffer to color specified
	device.ClearRenderTarget(pBackBufView, color);

	// Create and assign constant bufer
	ConstantBuffer cb(device, color);
	device.VSSetConstantBuffer(cb);

	// Assign vertex buffer to first slot
	device.IASetVertexBuffer(vBuf);
	// Assign index buffer
	device.IASetIndexBuffer(iBuf);

	// Compile and assign VS
	ComPtr<ID3DBlob> pBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(L"DefaultVertShader.cso", &pBlob));
	device.VSSetShader(device.CreateVertexShader(pBlob));

	// Define position semantic
	const UniqueArray<D3D11_INPUT_ELEMENT_DESC> layoutDesc =
	{
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"Color", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(vec2), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	const ComPtr<ID3D11InputLayout> pIALayout = device.CreateInputLayout(layoutDesc, pBlob);
	device.IASetInputLayout(pIALayout);

	// Compile and assign PS
	D3DReadFileToBlob(L"DefaultPixShader.cso", &pBlob);
	device.PSSetShader(device.CreatePixelShader(pBlob));
	
	// Set viewport bounds
	device.RSSetViewport(ivec2(640, 480));

	// Bind back buffer as render target
	device.OMSetRenderTarget(pBackBufView);

	device.DrawIndexed((UINT)iBuf.GetLength());

	// Present frame
	swap.Present(1u, 0);
}
