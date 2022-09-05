#include <math.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sstream>
#include "resource.h"

#include "MinWindow.hpp"
#include "Input.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/DebugScene.hpp"

using namespace Replica::D3D11;
using namespace std::chrono;
using namespace glm;

struct Vertex
{
public:
	vec3 pos;
};

DebugScene::DebugScene(Renderer& renderer, InputComponent& input) :
	RenderComponentBase(renderer),
	testTex(Texture2D::FromImageWIC(renderer.GetDevice(), L"lena_color_512.png")),
	testSamp(renderer.GetDevice(), TexFilterMode::LINEAR, TexClampMode::MIRROR),
	testEffect(renderer.GetDevice(), g_DefaultEffect),
	input(input)
{
	Device& dev = renderer.GetDevice();
	const MeshDef<Vertex> sphere = Primitives::GenerateSphere<Vertex>(ivec2(36)),
		cube = Primitives::GenerateCube<Vertex>(ivec2(36)),
		cone = Primitives::GenerateCone<Vertex>(36),
		cylinder = Primitives::GenerateCylinder<Vertex>(36),
		plane = Primitives::GeneratePlane<Vertex>(ivec2(36));

	primitives.emplace_back(dev, sphere);
	primitives.emplace_back(dev, cube);
	primitives.emplace_back(dev, cone);
	primitives.emplace_back(dev, cylinder);
	primitives.emplace_back(dev, plane);

	primitives[0].SetTranslation(vec3(0, 0, 0));
	primitives[1].SetTranslation(vec3(-1.0f, 0.7, 0));
	primitives[2].SetTranslation(vec3(-1.0f, -0.7, 0));
	primitives[3].SetTranslation(vec3(1.0f, 0.7, 0));
	primitives[4].SetTranslation(vec3(1.0f, -0.7, 0));
}

void DebugScene::Draw(Context& ctx)
{
	MinWindow& const window = pRenderer->GetParent();
	const ivec2 mousePos = input.GetMousePos();
	const vec2 normMousePos = input.GetNormMousePos(),
		clipMousePos = 2.0f * normMousePos + vec2(-1, 1);

	// Write debug info to window header
	std::wstringstream ss;
	ss << "Space: " << input.GetIsKeyPressed(KbKey::Space)
		<< "  Mouse: " << mousePos.x << ", " << mousePos.y
		<< "  MouseNorm: " << normMousePos.x << ", " << normMousePos.y;
	window.SetWindowTitle(ss.str().c_str());

	fquat camRot = QuatFromAxis(vec3(0, 0, 1), -0.4f * g_Pi);
	mat4 view = identity<mat4>();
	view = translate(view, vec3(0.0f, 0.0f, 5.0f));
	view *= toMat4(camRot);

	const ivec2 wndSize = window.GetSize();
	const float aspectRatio = (float)wndSize.x / wndSize.y;
	const mat4 proj = perspectiveLH(45.0f, aspectRatio, 0.5f, 100.0f);
	const mat4 vp = (proj * view);

	const duration<double> time = duration_cast<duration<double>>(steady_clock::now().time_since_epoch());
	fquat meshRot = QuatFromAxis(vec3(0, 1, 0), g_Tau * normMousePos.x);
	meshRot = QuatFromAxis(vec3(0, 0, 1), g_Tau * normMousePos.y) * meshRot;

	for (int i = 0; i < primitives.GetLength(); i++)
	{
		Mesh& mesh = primitives[i];
		mesh.SetRotation(meshRot);

		float phaseOffset = i * g_Tau / (float)primitives.GetLength();
		vec2 animOffset(cos(time.count() + phaseOffset), sin(time.count() + phaseOffset));
		mesh.SetTranslation(vec3(2.0f * animOffset, 0.0f));

		mat4 model = mesh.GetModelMatrix();
		testEffect.SetConstant(L"mvp", vp * model);
		testEffect.SetSampler(L"samp", testSamp);
		testEffect.SetTexture(L"tex", testTex);
		testEffect.SetConstant(L"DstTexelSize", vec4(1.0f / vec2(wndSize), vec2(wndSize)));

		testEffect.Update(ctx);
		mesh.Update(ctx);
		mesh.Draw(ctx);
	}
}
