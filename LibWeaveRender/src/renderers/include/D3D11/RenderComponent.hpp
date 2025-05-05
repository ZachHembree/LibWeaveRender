#pragma once
#include "WeaveUtils/Utils.hpp"

namespace Weave
{
	class MinWindow;
}

namespace Weave::D3D11
{
	class CtxImm;
	class Renderer;
	class Device;

	/// <summary>
	/// Base class for Renderer components
	/// </summary>
	class RenderComponentBase
	{
	friend Renderer;

	public:
		MAKE_IMMOVABLE(RenderComponentBase)

		RenderComponentBase(Renderer& renderer, uint priority = 10);

		virtual ~RenderComponentBase() = 0;

		/// <summary>
		/// Updates before draw, but after the state and resources for the previous
		/// frame have been cleaned up.
		/// </summary>
		virtual void Setup(CtxImm& ctx) { }

		virtual void Draw(CtxImm& ctx) { }

		/// <summary>
		/// Called after swap chain present but before any resources have been released
		/// or cleaned up.
		/// </summary>
		virtual void AfterDraw(CtxImm& ctx) { }

		/// <summary>
		/// Returns reference to the renderer this component is attached to
		/// </summary>
		Renderer& GetRenderer();

		/// <summary>
		/// Returns device interface to the renderer
		/// </summary>
		Device& GetDevice();

		/// <summary>
		/// Returns interface reference to window being rendered to
		/// </summary>
		MinWindow& GetWindow();

		/// <summary>
		/// Returns reference to the renderer this component is attached to
		/// </summary>
		const Renderer& GetRenderer() const;

		/// <summary>
		/// Returns device interface to the renderer
		/// </summary>
		const Device& GetDevice() const;

		/// <summary>
		/// Returns interface reference to window being rendered to
		/// </summary>
		const MinWindow& GetWindow() const;

		/// <summary>
		/// Returns true if the component has been registered to a renderer
		/// </summary>
		bool GetIsRegistered(Renderer* pRenderer = nullptr) const;

		/// <summary>
		/// Returns the update priority set for the component on construction. Components with higher priority values are 
		/// updated later. Components with a priority of 0 update soonest.
		/// </summary>
		uint GetPriority() const;

	private:
		Renderer* pRenderer;
		uint id;
		uint priority;
	};

	inline RenderComponentBase::~RenderComponentBase() {}
}