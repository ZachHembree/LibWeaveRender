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

	/// <summary>
	/// Determines how vertex topology is interpreted by the input assembler
	/// </summary>
	enum class PrimTopology
	{
		UNDEFINED = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,
		POINTLIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINELIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINESTRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLELIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLESTRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		LINELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
		LINESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
		TRIANGLELIST_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
		TRIANGLESTRIP_ADJ = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
		CP1_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
		CP2_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
		CP3_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		CP4_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
		CP5_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST,
		CP6_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST,
		CP7_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST,
		CP8_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST,
		CP9_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST,
		CP10_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST,
		CP11_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST,
		CP12_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST,
		CP13_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST,
		CP14_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST,
		CP15_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST,
		CP16_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST,
		CP17_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST,
		CP18_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST,
		CP19_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST,
		CP20_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST,
		CP21_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST,
		CP22_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST,
		CP23_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST,
		CP24_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST,
		CP25_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST,
		CP26_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST,
		CP27_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST,
		CP28_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST,
		CP29_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST,
		CP30_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST,
		CP31_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST,
		CP32_PATCHLIST = D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST
	};

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