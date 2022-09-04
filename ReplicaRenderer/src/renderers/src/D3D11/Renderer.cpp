#include <math.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sstream>
#include "resource.h"

#include "MinWindow.hpp"

#include "D3D11/Renderer.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Effect.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

using namespace glm;
using namespace std::chrono;
using namespace Replica;
using namespace Replica::D3D11;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Vertex
{
public:
	vec3 pos;
};

Renderer::Renderer(MinWindow* window) :
	WindowComponentBase(window),
	input(window),
	device(), // Create device and context
	swap(*window, &device), // Create swap chain for window
	backBuf(swap.GetBuffer(0)), // Get RT view for swap chain back buf
	testTex(Texture2D::FromImageWIC(& device, L"lena_color_512.png")),
	testSamp(&device, TexFilterMode::LINEAR, TexClampMode::MIRROR),
	testEffect(device, g_DefaultEffect)
{
	ImGui_ImplDX11_Init(device.Get(), device.GetContext().Get());
	const MeshDef<Vertex> sphere = Primitives::GenerateSphere<Vertex>(ivec2(36)),
		cube = Primitives::GenerateCube<Vertex>(ivec2(36)),
		cone = Primitives::GenerateCone<Vertex>(36),
		cylinder = Primitives::GenerateCylinder<Vertex>(36),
		plane = Primitives::GeneratePlane<Vertex>(ivec2(36));

	scene.emplace_back(device, sphere);
	scene.emplace_back(device, cube);
	scene.emplace_back(device, cone);
	scene.emplace_back(device, cylinder);
	scene.emplace_back(device, plane);

	scene[0].SetTranslation(vec3(0, 0, 0));
	scene[1].SetTranslation(vec3(-1.0f, 0.7, 0));
	scene[2].SetTranslation(vec3(-1.0f, -0.7, 0));
	scene[3].SetTranslation(vec3(1.0f, 0.7, 0));
	scene[4].SetTranslation(vec3(1.0f, -0.7, 0));
}

void Renderer::Update()
{
	const ivec2 wndSize = parent->GetSize();
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

	fquat camRot = QuatFromAxis(vec3(0, 0, 1), -0.4f * g_Pi);
	mat4 view = identity<mat4>();
	view = translate(view, vec3(0.0f, 0.0f, 5.0f));
	view *= toMat4(camRot);

	const float aspectRatio = (float)wndSize.x / wndSize.y;
	const mat4 proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);
	const mat4 vp = (proj * view);

	// Clear back buffer
	backBuf.Clear(ctx);

	// Set viewport bounds
	ctx.RSSetViewport(parent->GetSize());
	// Bind back buffer as render target
	backBuf.Bind(ctx);

	const duration<double> time = duration_cast<duration<double>>(steady_clock::now().time_since_epoch());
	fquat meshRot = QuatFromAxis(vec3(0, 1, 0), g_Tau * normMousePos.x);
	meshRot = QuatFromAxis(vec3(0, 0, 1), g_Tau * normMousePos.y) * meshRot;

	for (int i = 0; i < scene.GetLength(); i++)
	{
		Mesh& mesh = scene[i];
		mesh.SetRotation(meshRot);
		
		float phaseOffset = i * g_Tau / (float)scene.GetLength();
		vec2 animOffset(cos(time.count() + phaseOffset), sin(time.count() + phaseOffset));
		mesh.SetTranslation(vec3(2.0f * animOffset, 0.0f));

		mat4 model = mesh.GetModelMatrix();
		testEffect.SetConstant(L"mvp", vp * model);
		testEffect.SetSampler(L"samp", testSamp);
		testEffect.SetTexture(L"tex", testTex);
		testEffect.SetConstant(L"DstTexelSize", vec4(1.0f / vec2(wndSize), vec2(wndSize)));

		testEffect.Setup(ctx);
		mesh.Setup(ctx);
		mesh.Draw(ctx);
	}

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