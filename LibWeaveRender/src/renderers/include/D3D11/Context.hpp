#pragma once
#include "Resources/DeviceChild.hpp"
#include "Resources/ResourceBase.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "Viewport.hpp"
#include "Resources/ResourceMap.hpp"

namespace Weave::D3D11
{
	using Effects::ShadeStages;
	class Renderer;

	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;

	class SwapChain;
	class InputLayout;

	class VertexShaderVariant;
	class PixelShaderVariant;
	class ComputeShaderVariant;
	class Material;
	class ResourceSet;

	class Texture2D;
	class RWTexture2D;
	class Mesh;

	class IRenderTarget;
	class IDepthStencil;

	struct Viewport;

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
		void BindShader(const VertexShaderVariant& vs, const ResourceSet& res);

		/// <summary>
		/// Binds the given pixel shader
		/// </summary>
		void BindShader(const PixelShaderVariant& ps, const ResourceSet& res);

		/// <summary>
		/// Binds the given compute shader
		/// </summary>
		void BindShader(const ComputeShaderVariant& cs, const ResourceSet& res);

		/// <summary>
		/// Clears the given vertex shader
		/// </summary>
		void UnbindShader(ShadeStages stage);

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsBound(const VertexShaderVariant* vs) const;

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsBound(const PixelShaderVariant* ps) const;

		/// <summary>
		/// Returns true if the given shader is bound
		/// </summary>
		bool GetIsBound(const ComputeShaderVariant* cs) const;

		void Dispatch(const ComputeShaderVariant& cs, ivec3 groups, const ResourceSet& res);

		void SetSamplers(const SamplerMap::DataSrc& sampMap, const SamplerMap& map, ShadeStages stage);

		void SetUAVs(const UnorderedAccessMap::DataSrc& uavs, const UnorderedAccessMap& map);

		void SetSRVs(const ResourceViewMap::DataSrc& srvMap, const ResourceViewMap& map, ShadeStages stage);

		void SetConstants(const IDynamicArray<Span<byte>>& cbufData, IDynamicArray<ConstantBuffer>& cBuffers, ShadeStages stage);

		void ClearSamplers(const uint start, const uint count, ShadeStages stage);

		void ClearUAVs(const uint start, const uint count);

		void ClearSRVs(const uint start, const uint count, ShadeStages stage);

		void ClearConstants(const uint start, const uint count, ShadeStages stage);

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
		/// Returns the number of viewports currently bound
		/// </summary>
		const int GetViewportCount() const;

		/// <summary>
		/// Returns the viewport bound to the given index.
		/// </summary>
		const Viewport& GetViewport(int index) const;

		/// <summary>
		/// Returns an array of the viewports currently bound
		/// </summary>
		const IDynamicArray<Viewport>& GetViewports() const;

		/// <summary>
		/// Binds the given collection of viewports to the rasterizer stage
		/// </summary>
		void SetViewports(const IDynamicArray<Viewport>& viewports, int offset = 0);

		/// <summary>
		/// Binds the given viewport to the rasterizer stage
		/// </summary>
		void SetViewport(int index, const vec2& size, const vec2& offset = vec2(0, 0), const vec2& depth = vec2(0, 1));

		/// <summary>
		/// Binds the given viewport to the rasterizer stage
		/// </summary>
		void SetViewport(int index, const Viewport& vp);

		/// <summary>
		/// Binds the given buffer as the depth stencil buffer doesn't overwrite render targets. Set to nullptr
		/// to unbind.
		/// </summary>
		void SetDepthStencilBuffer(IDepthStencil& depthStencil);

		/// <summary>
		/// Returns the number of render targets currently bound
		/// </summary>
		int GetRenderTargetCount() const;

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
		void SetRenderTargets(IDynamicArray<IRenderTarget>& rts, IDepthStencil& ds);

		/// <summary>
		/// Binds the given buffers as render targets. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void SetRenderTargets(IDynamicArray<IRenderTarget>& rts, IDepthStencil* pDS = nullptr);

		/// <summary>
		/// Binds a vertex buffer to the given slot
		/// </summary>
		void IASetVertexBuffer(VertexBuffer& vertBuffer, int slot = 0);

		/// <summary>
		/// Determines how vertices are interpreted by the input assembler.
		/// Defaults to PrimTopology::TRIANGLELIST.
		/// </summary>
		void IASetPrimitiveTopology(PrimTopology topology);

		/// <summary>
		/// Binds an array of buffers starting at the given slot
		/// </summary>
		void IASetVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, int startSlot = 0);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, IRWTexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, IResizeableTexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox = ivec4(0), ivec4 dstBox = ivec4(0));

		/// <summary>
		/// Copies the contents of a texture to a render target
		/// </summary>
		void Blit(IResizeableTexture2D& src, IRenderTarget& dst);

		/// <summary>
		/// Copies the contents of a texture to a render target
		/// </summary>
		void Blit(ITexture2D& src, IRenderTarget& dst, ivec4 srcBox = ivec4(0));

		/// <summary>
		/// Draws an indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(Mesh& mesh, Material& mat);

		/// <summary>
		/// Draws a group of indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(IDynamicArray<Mesh>& meshes, Material& mat);

	private:
		ComPtr<ID3D11DeviceContext> pContext;

		const VertexShaderVariant* currentVS;
		const PixelShaderVariant* currentPS;
		const ComputeShaderVariant* currentCS;

		ID3D11DepthStencilState* currentDSS;
		ID3D11DepthStencilView* currentDSV;
		vec2 currentDepthRange;

		UniqueArray<Viewport> currentVPs;
		Span<Viewport> vpSpan;
		uint vpCount;

		UniqueArray<ID3D11RenderTargetView*> currentRTVs;
		Span<ID3D11RenderTargetView*> rtvSpan;
		uint rtvCount;

		/// <summary>
		/// Returns the render target view bound to the given index.
		/// </summary>
		ID3D11RenderTargetView* GetRenderTarget(int index) const;

		/// <summary>
		/// Returns an array of the render target views currently bound
		/// </summary>
		const IDynamicArray<ID3D11RenderTargetView*>& GetRenderTargets() const;
	};
}