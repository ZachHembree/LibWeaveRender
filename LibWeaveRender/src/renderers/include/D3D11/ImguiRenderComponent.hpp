#pragma once
#include "RenderComponent.hpp"
#include "WeaveUtils/TextUtils.hpp"
#include "WeaveUtils/ObjectPool.hpp"

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

		ImguiRenderComponent(Renderer& renderer);

		~ImguiRenderComponent();

		string& GetTempString();

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

		void Setup(CtxImm& ctx) override;

		void DrawLate(CtxImm& ctx) override;

	private:
		vec2 mousePos;
		uivec2 dispSize;

		ObjectPool<string> stringPool;
		UniqueVector<string> activeText;
	};
}