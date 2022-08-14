#include "D3D11/Renderer.hpp"
#include <math.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace glm;
using namespace Replica;
using namespace Replica::D3D11;

struct Vertex
{
public:
	vec3 pos;
	tvec4<byte> color;
};

Renderer::Renderer(MinWindow* window) :
	WindowComponentBase(window),
	input(window),
	device(), // Create device and context
	swap(*window, device), // Create swap chain for window
	pBackBufView(device.GetRtView(swap.GetBuffer(0))), // Get RT view for swap chain back buf
	vBuf(device, UniqueArray<Vertex>{
		{ { -1.0, -1.0, -1.0 }, { 255, 0, 0, 255 } },
		{ { 1.0, -1.0, -1.0 }, { 0, 255, 0, 255 } },
		{ { -1.0, 1.0, -1.0 }, { 0, 0, 255, 255  } },
		{ { 1.0, 1.0, -1.0 }, { 0, 255, 0, 255 } }, // 3
		{ { -1.0, -1.0, 1.0 }, { 255, 0, 0, 255 } },
		{ { 1.0, -1.0, 1.0 }, { 255, 0, 0, 255 } },
		{ { -1.0, 1.0, 1.0 }, { 0, 255, 0, 255 } },
		{ { 1.0, 1.0, 1.0 }, { 0, 0, 255, 255  } },
	}),
	iBuf(device, UniqueArray<USHORT>{
		0, 2, 1,  2, 3, 1,
		1, 3, 5,  3, 7, 5,
		2, 6, 3,  3, 6, 7,
		4, 5, 7,  4, 7, 6,
		0, 4, 2,  2, 4, 6,
		0, 1, 4,  1, 5, 4
	})
{ 
	
}

void Renderer::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	const ivec2 wndSize = parent->GetSize();
	const float sinOffset = sin(time.count() * .5f),
		cosOffset = cos(time.count() * .5f),
		aspectRatio = (float)wndSize.x / wndSize.y;
	const vec2 normMousePos = input.GetNormMousePos(),
		clipMousePos = 2.0f * normMousePos + vec2(-1, 1);

	struct
	{
		vec4 tint;
		mat4 mvp;
	} cBuf{};
	
	cBuf.tint = vec4(std::abs(sinOffset), std::abs(cosOffset), std::abs(sinOffset + cosOffset), 1.0f);
	
	mat4 model = identity<mat4>(),
		view = identity<mat4>(),
		proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);

	model = translate(model, vec3(0.0f, 0.0f, 4.0f));
	model = rotate(model, normMousePos.x * pi<float>(), normalize(vec3(0.0f, 1.0f, 0.0f)));
	model = rotate(model, normMousePos.y * pi<float>(), normalize(vec3(0.0f, 0.0f, 1.0f)));
	//model = scale(model, vec3(.75f));
	
	//view = translate(view, vec3(clipMousePos, 0.0f));
	//view = rotate(view, 0.1f * pi<float>() * cosOffset, normalize(vec3(0.25f, 0.5f, 1.0f)));

	// D3D expects row major matrices
	cBuf.mvp = transpose(proj * view * model);

	// Clear back buffer to color specified
	device.ClearRenderTarget(pBackBufView, vec4(0));

	// Create and assign constant bufer
	ConstantBuffer cb(device, cBuf);
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
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(vec3), D3D11_INPUT_PER_VERTEX_DATA, 0}
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
