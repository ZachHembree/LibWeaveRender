#pragma once
#include "D3DUtils.hpp"

namespace Replica::D3D11
{
	class Context;
	class Renderer;

	/// <summary>
	/// Interface for types that can be drawn
	/// </summary>
	class RenderComponentBase : private MoveOnlyObjBase
	{
	friend Renderer;

	public:
		RenderComponentBase(RenderComponentBase&&) = delete;
		RenderComponentBase& operator=(RenderComponentBase&&) = delete;

		virtual void Update(Context& ctx) { }

		virtual void DrawEarly(Context& ctx) { }

		virtual void Draw(Context& ctx) { }

		virtual void DrawLate(Context& ctx) { }

		virtual void AfterDraw(Context& ctx) { }

		Renderer* GetRenderer() { return pRenderer; }

		bool GetIsRegistered() { return isRegistered; }

		virtual void Register(Renderer& renderer);

		virtual void Unregister();

	protected:
		Renderer* pRenderer;
		bool isRegistered;

		RenderComponentBase() : pRenderer(nullptr), isRegistered(false)
		{ }

		RenderComponentBase(Renderer& renderer) : pRenderer(nullptr), isRegistered(false)
		{
			Register(renderer);
		}

		~RenderComponentBase()
		{
			Unregister();
		}
	};
}