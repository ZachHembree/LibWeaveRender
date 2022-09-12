#pragma once
#include "RenderComponent.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Wrapper for standard D3D11 Imgui implementation
	/// </summary>
	class ImguiRenderComponent : public RenderComponentBase
	{
	public:
		bool enableDemoWindow;

		ImguiRenderComponent();

		ImguiRenderComponent(Renderer& renderer);

		~ImguiRenderComponent();

		void Setup(Context& ctx) override;

		void DrawLate(Context& ctx) override;

	private:
		
	};
}