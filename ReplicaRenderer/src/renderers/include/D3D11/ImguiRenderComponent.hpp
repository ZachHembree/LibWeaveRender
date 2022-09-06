#pragma once
#include "D3D11/RenderComponent.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Wrapper for standard D3D11 Imgui implementation
	/// </summary>
	class ImguiRenderComponent : public RenderComponentBase
	{
	public:
		ImguiRenderComponent();

		ImguiRenderComponent(Renderer& renderer);

		~ImguiRenderComponent();

		void Update(Context& ctx) override;

		void DrawLate(Context& ctx) override;

	private:
		
	};
}