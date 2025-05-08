#pragma once
#include <array>
#include <functional>
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/ComponentManagerBase.hpp"

namespace Weave
{
	class MinWindow;
}

namespace Weave::D3D11
{
	class CtxImm;
	class Renderer;
	class Device;

	enum class RenderStages : byte
	{
		Setup = 0,
		Draw = 1,
		AfterDraw = 2,
		Count
	};

	/// <summary>
	/// Base class for Renderer components
	/// </summary>
	class RenderComponentBase : public ComponentManagerBase<Renderer, RenderComponentBase>::ComponentBase
	{
	public:
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

	protected:
		RenderComponentBase(Renderer& renderer, uint priority = 10);

	};

	/// <summary>
	/// Generic render component wrapper that provides std::function hooks for callbacks 
	/// into render updates
	/// </summary>
	class RenderHook : public RenderComponentBase
	{
	public:
		/// <summary>
		/// Type erased function pointer callback alias
		/// </summary>
		using CallbackT = std::function<void(CtxImm&)>;

		RenderHook(Renderer& renderer, uint priority = 10);

		/// <summary>
		/// Returns the callback function used for the given render stage
		/// </summary>
		const CallbackT& GetCallback(RenderStages stage) const;

		/// <summary>
		/// Sets the callback to be invoked when the given render stage update occurs
		/// </summary>
		void SetCallback(RenderStages stage, CallbackT&& func);

	private:
		std::array<CallbackT, (byte)RenderStages::Count> callbacks;

		void Setup(CtxImm& ctx) override;

		void Draw(CtxImm& ctx) override;

		void AfterDraw(CtxImm& ctx) override;
	};
}