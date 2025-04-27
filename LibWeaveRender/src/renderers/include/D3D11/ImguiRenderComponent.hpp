#pragma once
#include "RenderComponent.hpp"
#include "Input.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Wrapper for standard D3D11 Imgui implementation
	/// </summary>
	class ImguiRenderComponent : public RenderComponentBase
	{
	public:
		bool enableDemoWindow;

		ImguiRenderComponent();

		ImguiRenderComponent(Renderer& renderer, InputHandler& input);

		~ImguiRenderComponent();

		void Setup(CtxImm& ctx) override;

		void DrawLate(CtxImm& ctx) override;

	private:
		InputHandler* pInput;
	};
}