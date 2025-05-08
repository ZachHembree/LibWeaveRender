#pragma once
#include "RenderComponent.hpp"
#include "WeaveUtils/TextUtils.hpp"
#include "WeaveUtils/ObjectPool.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Wrapper for standard D3D11 ImGui backend
	/// </summary>
	class ImGuiRenderComponent : public RenderComponentBase
	{
	public:
		/// <summary>
		/// Enables or disables the built-in ImGui demo UI
		/// </summary>
		bool enableDemoWindow;

		ImGuiRenderComponent(Renderer& renderer);

		~ImGuiRenderComponent();

		/// <summary>
		/// Returns overridden mouse position. Zero if disabled.
		/// </summary>
		vec2 GetMousePos() const;

		/// <summary>
		/// Returns overridden display size. Zero if disabled.
		/// </summary>
		uivec2 GetDispSize() const;

		/// <summary>
		/// Sets override for mouse position. Must be updated every frame or set to zero to disable.
		/// </summary>
		void SetMousePos(vec2 mousePos);

		/// <summary>
		/// Sets override for display size. Must be updated every frame or set to zero to disable.
		/// </summary>
		void SetDispSize(uivec2 dispSize);

	private:
		vec2 mousePos;
		uivec2 dispSize;

		void Setup(CtxImm& ctx) override;

		void Draw(CtxImm& ctx) override;

	};
}