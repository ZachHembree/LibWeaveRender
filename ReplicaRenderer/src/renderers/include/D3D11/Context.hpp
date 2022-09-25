#pragma once
#include "ReplicaD3D11.hpp"
#include "Resources/DeviceChild.hpp"
#include "Resources/ResourceBase.hpp"

namespace Replica::D3D11
{
	class Renderer;

	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class SwapChain;
	class InputLayout;

	class VertexShader;
	class PixelShader;
	class ComputeShader;
	class Effect;

	class Texture2D;
	class RWTexture2D;
	class Mesh;

	class IRenderTarget;
	class IDepthStencil;

	enum class PrimTopology;

	/// <summary>
	/// Class for issuing commands to an associated D3D11 device
	/// </summary>
	class Context : public DeviceChild
	{
	public:
		Context(Device& dev, ComPtr<ID3D11DeviceContext>& pContext);

		Context();

		Context(Context&&) = default;

		Context& operator=(Context&&) = default;

		/// <summary>
		/// Returns reference to renderer using this context
		/// </summary>
		Renderer& GetRenderer();

		/// <summary>
		/// Binds the given vertex shader
		/// </summary>
		void SetVS(VertexShader* vs);

		/// <summary>
		/// Binds the given pixel shader
		/// </summary>
		void SetPS(PixelShader* ps);

		/// <summary>
		/// Binds the given compute shader
		/// </summary>
		void SetCS(ComputeShader* cs);

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsVsBound(VertexShader* vs) const;

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsPsBound(PixelShader* ps) const;

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsCsBound(ComputeShader* cs) const;

		/// <summary>
		/// Unbinds all resources
		/// </summary>
		void Reset();

		/// <summary>
		/// Returns reference to context interface
		/// </summary>
		ID3D11DeviceContext& Get() const;
		
		/// <summary>
		/// Returns reference to context interface
		/// </summary>
		ID3D11DeviceContext* operator->() const;

		/// <summary>
		/// Binds the given viewport to the rasterizer stage
		/// </summary>
		void RSSetViewport(const vec2 size, const vec2 offset = vec2(0, 0), const vec2 depth = vec2(0, 1));

		/// <summary>
		/// Binds the given buffer as the depth stencil buffer doesn't overwrite render targets. Set to nullptr
		/// to unbind.
		/// </summary>
		void SetDepthStencilBuffer(IDepthStencil& depthStencil);

		/// <summary>
		/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void SetRenderTarget(IRenderTarget& rt, IDepthStencil& ds);

		/// <summary>
		/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void SetRenderTarget(IRenderTarget& rt, IDepthStencil* pDS = nullptr);

		/// <summary>
		/// Binds the given buffers as render targets. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void SetRenderTargets(const IDynamicArray<IRenderTarget>& rts, IDepthStencil& ds);

		/// <summary>
		/// Binds the given buffers as render targets. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void SetRenderTargets(const IDynamicArray<IRenderTarget>& rts, IDepthStencil* pDS = nullptr);

		/// <summary>
		/// Binds a vertex buffer to the given slot
		/// </summary>
		void IASetVertexBuffer(VertexBuffer& vertBuffer, int slot = 0);

		/// <summary>
		/// Determines how vertices are interpreted by the input assembler
		/// </summary>
		void IASetPrimitiveTopology(PrimTopology topology);

		/// <summary>
		/// Binds an array of buffers starting at the given slot
		/// </summary>
		void IASetVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, int startSlot = 0);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IRWTexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, ITexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IRenderTarget& dst);

		/// <summary>
		/// Draws an indexed, non-instanced triangle meshes using the given effect
		/// </summary>
		void Draw(Mesh& mesh, Effect& effect);

		/// <summary>
		/// Draws a group of indexed, non-instanced triangle meshes using the given effect
		/// </summary>
		void Draw(IDynamicArray<Mesh>& meshes, Effect& effect);

	private:
		ComPtr<ID3D11DeviceContext> pContext;

		VertexShader* currentVS;
		PixelShader* currentPS;
		ComputeShader* currentCS;

		ID3D11DepthStencilState* currentDSS;
		ID3D11DepthStencilView* currentDSV;
		UniqueArray<ID3D11RenderTargetView*> currentRTVs;
		uint rtvCount;
	};
}