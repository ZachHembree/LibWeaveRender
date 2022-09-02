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
#include "D3D11/dev/ConstantMap.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Primitives.hpp"

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
	vec2 uv;
};

const VertexShaderDef g_DefaultVS = 
{
	ShaderDefBase
	{
		{ ConstantDef::Get<mat4>(L"mvp"), }
	},
	L"DefaultVertShader.cso",
	{
		{ "Position", Formats::R32G32B32_FLOAT },
		{ "TexCoord", Formats::R32G32_FLOAT },
	},
};

const PixelShaderDef g_DefaultPS = 
{
	ShaderDefBase
	{
		{
			{ ConstantDef::Get<vec4>(L"DstTexelSize"), }
		},
		{ L"samp" },
		{ L"tex" }
	},
	L"DefaultPixShader.cso",
};

const EffectDef g_DefaultEffect =
{
	g_DefaultVS,
	g_DefaultPS
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

	scene.emplace_back(Primitives::GenerateCylinder<Vertex>(device, 8));
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

	mat4 view = identity<mat4>(),
		proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);

	fquat rot = QuatFromAxis(vec3(0, 1, 0), 2 * pi<float>() * normMousePos.x);
	rot = QuatFromAxis(vec3(0, 0, 1), 2 * pi<float>() * normMousePos.y) * rot;

	const mat4 vp = (proj * view);

	// Clear back buffer
	backBuf.Clear(ctx);

	// Set viewport bounds
	ctx.RSSetViewport(parent->GetSize());
	// Bind back buffer as render target
	backBuf.Bind(ctx);

	for (int i = 0; i < scene.GetLength(); i++)
	{
		Mesh& mesh = scene[i];
		mesh.SetTranslation(vec3(0, 0, 3));
		mesh.SetRotation(rot);

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