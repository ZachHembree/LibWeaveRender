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

using namespace glm;
using namespace std::chrono;
using namespace Replica;
using namespace Replica::D3D11;

struct Vertex
{
public:
	vec3 pos;
};

Renderer::Renderer(MinWindow& window) :
	WindowComponentBase(window),
	input(window),
	device(), // Create device and context
	swap(window, device), // Create swap chain for window
	backBuf(swap.GetBuffer(0)), // Get RT view for swap chain back buf
	testTex(Texture2D::FromImageWIC(& device, L"lena_color_512.png")),
	testSamp(&device, TexFilterMode::LINEAR, TexClampMode::MIRROR),
	testEffect(device, g_DefaultEffect)
{
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
	// Reset binds
	Context& ctx = device.GetContext();
	ctx.Reset();

	// Clear back buffer
	backBuf.Clear(ctx);

	BeforeDraw(ctx);

	// Set viewport bounds
	ctx.RSSetViewport(GetParent().GetSize());
	// Bind back buffer as render target
	backBuf.Bind(ctx);

	DrawEarly(ctx);
	Draw(ctx);
	DrawLate(ctx);

	// Present frame
	swap.Present(1u, 0);

	AfterDraw(ctx);
}

void Renderer::BeforeDraw(Context& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Update(ctx);
	}
}

void Renderer::DrawEarly(Context& ctx)
{ 
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawEarly(ctx);
	}
}

void Renderer::Draw(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->Draw(ctx);
	}

	const ivec2 wndSize = GetParent().GetSize();
	const ivec2 mousePos = input.GetMousePos();
	const vec2 normMousePos = input.GetNormMousePos(),
		clipMousePos = 2.0f * normMousePos + vec2(-1, 1);

	// Write debug info to window header
	std::wstringstream ss;
	ss << "Space: " << input.GetIsKeyPressed(KbKey::Space)
		<< "  Mouse: " << mousePos.x << ", " << mousePos.y
		<< "  MouseNorm: " << normMousePos.x << ", " << normMousePos.y;
	GetParent().SetWindowTitle(ss.str().c_str());

	fquat camRot = QuatFromAxis(vec3(0, 0, 1), -0.4f * g_Pi);
	mat4 view = identity<mat4>();
	view = translate(view, vec3(0.0f, 0.0f, 5.0f));
	view *= toMat4(camRot);

	const float aspectRatio = (float)wndSize.x / wndSize.y;
	const mat4 proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);
	const mat4 vp = (proj * view);

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
}

void Renderer::DrawLate(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->DrawLate(ctx);
	}	
}

void Renderer::AfterDraw(Context& ctx)
{
	for (RenderComponentBase* pComp : pComponents)
	{
		pComp->AfterDraw(ctx);
	}
}

void Renderer::OnWndMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ }

/// <summary>
/// Registers a new render component. Returns false if the component is already registered.
/// </summary>
bool Renderer::RegisterComponent(RenderComponentBase& component)
{
	if (!component.GetIsRegistered())
	{
		component.pRenderer = this;
		component.isRegistered = true;
		pComponents.push_back(&component);
		return true;
	}
	else
		return false;
}

/// <summary>
/// Unregisters the given component from the renderer. Returns false on fail.
/// </summary>
bool Renderer::UnregisterComponent(RenderComponentBase& component)
{
	int index = -1;

	for (int i = pComponents.GetLength() - 1; i >= 0; i--)
	{
		if (pComponents[i] == &component)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		component.isRegistered = false;
		component.pRenderer = nullptr;
		pComponents.RemoveAt(index);
		return true;
	}
	else
		return false;
}
