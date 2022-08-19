#include <math.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sstream>

#include "MinWindow.hpp"

#include "D3D11/Renderer.hpp"
#include "D3D11/dev/VertexShader.hpp"
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/dev/Texture2D.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace glm;
using namespace std::chrono;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Replica;
using namespace Replica::D3D11;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Vertex
{
public:
	D3D11::vec3 pos;
	D3D11::vec2 uv;
};

fquat QuatFromAxis(vec3 axis, float rad)
{
	rad *= 0.5f;
	float a = sin(rad);

	return fquat(
		a * axis.x,
		a * axis.y,
		a * axis.z,
		cos(rad)
	);
}

Renderer::Renderer(MinWindow* window) :
	WindowComponentBase(window),
	input(window),
	device(), // Create device and context
	swap(*window, &device), // Create swap chain for window
	backBuf(swap.GetBuffer(0)), // Get RT view for swap chain back buf
	vBuf(device, UniqueArray<Vertex>{
		{ { -1.0, -1.0, -1.0 }, { 0.0, 0.0 } },
		{ { 1.0, -1.0, -1.0 }, { 1.0, 0.0 } },
		{ { -1.0, 1.0, -1.0 }, { 0.0, 1.0 } },
		{ { 1.0, 1.0, -1.0 }, { 1.0, 1.0 } }, // 3
		{ { -1.0, -1.0, 1.0 }, { 0.0, 0.0 } },
		{ { 1.0, -1.0, 1.0 }, { 1.0, 0.0 } },
		{ { -1.0, 1.0, 1.0 }, { 0.0, 1.0 } },
		{ { 1.0, 1.0, 1.0 }, { 1.0, 1.0 } },
	}),
	iBuf(device, UniqueArray<USHORT>{
		0, 2, 1,  2, 3, 1,
		1, 3, 5,  3, 7, 5,
		2, 6, 3,  3, 6, 7,
		4, 5, 7,  4, 7, 6,
		0, 4, 2,  2, 4, 6,
		0, 1, 4,  1, 5, 4
	}),
	testTex(Texture2D::FromImageWIC(& device, L"lena_color_512.png")),
	testSamp(&device, TexFilterMode::LINEAR, TexClampMode::MIRROR),
	cb(device, sizeof(mat4)),
	vs(device, L"DefaultVertShader.cso", 
	{
		{ "Position", Formats::R32G32B32_FLOAT },
		{ "TexCoord", Formats::R32G32_FLOAT },
	}),
	ps(device, L"DefaultPixShader.cso")
{
	ImGui_ImplDX11_Init(device.Get(), device.GetContext().Get());
}

Renderer::~Renderer()
{ }

void Renderer::Update()
{
	const duration<float> time = duration_cast<duration<float>>(steady_clock::now().time_since_epoch());
	const ivec2 wndSize = parent->GetSize();
	const float sinOffset = sin(time.count() * .5f),
		cosOffset = cos(time.count() * .5f),
		aspectRatio = (float)wndSize.x / wndSize.y;
	const ivec2 mousePos = input.GetMousePos();
	const vec2 normMousePos = input.GetNormMousePos(),
		clipMousePos = 2.0f * normMousePos + vec2(-1, 1);

	// Reset binds
	Context& ctx = device.GetContext();
	ctx.Reset();

	// Write debug info to window header
	std::wstringstream ss;
	ss << "Space: " << input.GetIsKeyPressed(KbKey::Space)
		<< "  Mouse: " << mousePos.x << ", " << mousePos.y
		<< "  MouseNorm: " << normMousePos.x << ", " << normMousePos.y;
	parent->SetWindowTitle(ss.str().c_str());

	// Generate draw matrix for test cube
	mat4 model = identity<mat4>(),
		view = identity<mat4>(),
		proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);

	model = translate(model, vec3(0.0f, 0.0f, 4.0f));

	fquat rot = QuatFromAxis(vec3(0, 1, 0), pi<float>() * normMousePos.x);
	rot = QuatFromAxis(vec3(0, 0, 1), pi<float>() * normMousePos.y) * rot;
	model *= toMat4(rot);

	// D3D expects row major matrices
	const mat4 mvp = transpose(proj * view * model);

	// Clear back buffer
	backBuf.Clear(ctx);

	// Update VS constant bufer
	cb.SetData(&mvp,ctx);

	// Assign VS
	vs.Bind();
	vs.SetConstants(cb);

	// Assign PS
	ps.Bind();
	ps.SetSampler(testSamp);
	ps.SetTexture(testTex);

	// Set viewport bounds
	ctx.RSSetViewport(parent->GetSize());
	// Bind back buffer as render target
	backBuf.Bind(ctx);

	// Bind and draw mesh data
	vBuf.Bind(ctx);
	iBuf.Bind(ctx);
	ctx.DrawIndexed((UINT)iBuf.GetLength());

	// IMGUI Test
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present frame
	swap.Present(1u, 0);
}

void Renderer::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}