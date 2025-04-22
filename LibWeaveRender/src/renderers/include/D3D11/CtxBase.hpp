#pragma once
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveUtils/Span.hpp"
#include "WeaveUtils/WinUtils.hpp"
#include "Resources/DeviceChild.hpp"
#include "Resources/ResourceBase.hpp"

struct ID3D11DeviceContext;

namespace Weave::D3D11
{
	using Effects::ShadeStages;

	class Renderer;
	class SwapChain;
	struct Viewport;
	class ContextState;

	enum class PrimTopology;
	class InputLayout;

	class IRenderTarget;
	class IDepthStencil;
	class Mesh;
	class Material;

	class BufferBase;
	class IndexBuffer;
	class VertexBuffer;
	class MappedBufferHandle;

	class ResourceSet;
	class ShaderVariantBase;
	class VertexShaderVariant;
	class ComputeShaderVariant;

	/// <summary>
	/// Abstract base class for immediate and deferred device contexts
	/// </summary>
	class CtxBase : public DeviceChild
	{
	public:
		MAKE_NO_COPY(CtxBase);

		using ActiveShaderSet = std::array<bool, 5>;

		/// <summary>
		/// Returns true if the context is immediate, or false if deferred.
		/// </summary>
		bool GetIsImmediate() const;

		/// <summary>
		/// Returns the number of viewports currently bound
		/// </summary>
		const uint GetViewportCount() const;

		/// <summary>
		/// Returns the viewport bound to the given index.
		/// </summary>
		const Viewport& GetViewport(uint index) const;

		/// <summary>
		/// Returns an array of the viewports currently bound
		/// </summary>
		const Span<Viewport> GetViewports() const;

		/// <summary>
		/// Binds the given buffer as the depth stencil buffer to the output merger
		/// </summary>
		void BindDepthStencilBuffer(IDepthStencil& depthStencil);

		/// <summary>
		/// Unbinds the give depth stencil buffer from the output merger
		/// </summary>
		void UnbindDepthStencilBuffer();

		/// <summary>
		/// Returns the number of render targets currently bound
		/// </summary>
		uint GetRenderTargetCount() const;

		/// <summary>
		/// Binds the given buffer as a render target.
		/// </summary>
		void BindRenderTarget(IRenderTarget& rt, IDepthStencil& ds, sint slot = 0);

		/// <summary>
		/// Binds the given buffer as a render target. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void BindRenderTarget(IRenderTarget& rt, sint slot = 0);

		/// <summary>
		/// Binds the given buffers as render targets. Doesn't unbind previously set depth-stencil buffers.
		/// </summary>
		void BindRenderTargets(IDynamicArray<IRenderTarget*>& rts, sint startSlot = 0);

		/// <summary>
		/// Binds the given buffers as render targets.
		/// </summary>
		void BindRenderTargets(IDynamicArray<IRenderTarget*>& rts, IDepthStencil* pDS, sint startSlot = 0);

		/// <summary>
		/// Unbinds the render target at the specified slot.
		/// </summary>
		void UnbindRenderTarget(sint slot);

		/// <summary>
		/// Unbinds the specified range of render targets. Unbinds depth stencil if completely cleared.
		/// </summary>
		void UnbindRenderTargets(sint startSlot = 0, uint count = (uint)-1);

		/// <summary>
		/// Determines how vertices are interpreted by the input assembler.
		/// </summary>
		void SetPrimitiveTopology(PrimTopology topology);

		/// <summary>
		/// Binds a vertex buffer to the given slot in the input assembler
		/// </summary>
		void BindVertexBuffer(VertexBuffer& vertBuffer, sint slot = 0);

		/// <summary>
		/// Binds an array of buffers starting at the given slot in the input assembler
		/// </summary>
		void BindVertexBuffers(IDynamicArray<VertexBuffer>& vertBuffers, sint startSlot = 0);

		/// <summary>
		/// Unbinds the vertex buffer at the specified slot in the input assembler
		/// </summary>
		void UnbindVertexBuffer(sint slot);

		/// <summary>
		/// Unbinds the specified range of vertex buffers from the context. If count == -1, then all
		/// buffers will be unbound.
		/// </summary>
		void UnbindVertexBuffers(sint startSlot = 0, uint count = (uint)-1);

		/// <summary>
		/// Binds an index buffer to the input assembler
		/// </summary>
		void BindIndexBuffer(IndexBuffer& indexBuffer, uint byteOffset = 0u);

		/// <summary>
		/// Unbinds the current index buffer
		/// </summary>
		void UnbindIndexBuffer();

		/// <summary>
		/// Returns true if the given shader is bound to the context
		/// </summary>
		bool GetIsBound(const ShaderVariantBase& shader) const;

		/// <summary>
		/// Binds the given shader to the appropriate stage with the given resources
		/// </summary>
		void BindShader(const ShaderVariantBase& shader, const ResourceSet& resSrc);

		/// <summary>
		/// Unbinds the shader from the context
		/// </summary>
		void UnbindShader(const ShaderVariantBase& shader);

		/// <summary>
		/// Unbinds the shader and resources in the given shading stage
		/// </summary>
		void UnbindStage(ShadeStages stage);

		/// <summary>
		/// Unbinds stages in the set marked as inactive
		/// </summary>
		void UnbindInactiveStages(ActiveShaderSet activeShaders);

		/// <summary>
		/// Dispatches a compute shader with the given number of thread groups and resources
		/// </summary>
		void Dispatch(const ComputeShaderVariant& cs, ivec3 groups, const ResourceSet& res);

		/// <summary>
		/// Draws an indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(Mesh& mesh, Material& mat);

		/// <summary>
		/// Draws a group of indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(IDynamicArray<Mesh>& meshes, Material& mat);

		/// <summary>
		/// Fully overwrites the contents of the buffer with the given data
		/// </summary>
		void SetBufferData(BufferBase& dst, const IDynamicArray<byte>& data);

		/// <summary>
		/// Writes data to the specified subregion. Data outside subregion undefined.
		/// </summary>
		void SetTextureData(ITexture2DBase& dst, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim, uivec2 dstOffset = uivec2(0));

		/// <summary>
		/// Returns the buffer handle and unmaps it from the CPU.
		/// </summary>
		void ReturnMappedBufferHandle(MappedBufferHandle&& handle);

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
		/// Clears the depth stencil texture to the given value
		/// </summary>
		void ClearDepthStencil(
			IDepthStencil& depthStencil, 
			DSClearFlags clearFlags = DSClearFlags::Depth,
			float depthClear = 1.0f,
			UINT8 stencilClear = 0
		);

		/// <summary>
		/// Clears the render target to the given color
		/// </summary>
		void ClearRenderTarget(IRenderTarget& rt, vec4 color = vec4(0));

		/// <summary>
		/// Unbinds all shaders and associated resources
		/// </summary>
		void ResetShaders();

		/// <summary>
		/// Resets the context to its default state
		/// </summary>
		void Reset();

	protected:
		ComPtr<ID3D11DeviceContext1> pCtx;
		std::unique_ptr<ContextState> pState;
		bool isImmediate;

		CtxBase();

		CtxBase(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx);

		CtxBase(CtxBase&&) noexcept;

		CtxBase& operator=(CtxBase&&) noexcept;

		/// <summary>
		/// Binds subtype-specific resources to the pipeline
		/// </summary>
		void BindResources(const VertexShaderVariant& shader, const ResourceSet& resSrc);

		/// <summary>
		/// Binds subtype-specific resources to the pipeline
		/// </summary>
		void BindResources(const ComputeShaderVariant& shader, const ResourceSet& resSrc);

		/// <summary>
		/// Resolves conflicts identified by the state cache by appropriately transitioning resource state or 
		/// raising an error.
		/// </summary>
		void HandleConflicts();
	};
}