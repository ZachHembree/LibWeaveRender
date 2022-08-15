#pragma once
#include "GfxException.hpp"
#include <d3d11.h>
#include <wrl.h>
#include <glm/glm.hpp>
#include "DynamicCollections.hpp"
#include "UniqueObj.hpp"

namespace Replica::D3D11
{
	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class SwapChain;
	class InputLayout;
	class VertexShader;
	struct IAElement;

	class Device : protected UniqueObjBase
	{
	public:
		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		class Child;

		Device();

		/// <summary>
		/// Returns pointer to COM device interface
		/// </summary>
		ID3D11Device* Get() const;

		/// <summary>
		/// Returns pointer to COM device context interface
		/// </summary>
		/// <returns></returns>
		ID3D11DeviceContext* GetContext() const;

		/// <summary>
		/// Creates an RT view for accessing resource data
		/// </summary>
		ComPtr<ID3D11RenderTargetView> GetRtView(const ComPtr<ID3D11Resource>& buffer);

		/// <summary>
		/// Clears the given render target to the given color
		/// </summary>
		void ClearRenderTarget(const ComPtr<ID3D11RenderTargetView>& rtView, glm::vec4 color);

		/// <summary>
		/// Binds the given viewport to the rasterizer stage
		/// </summary>
		void RSSetViewport(const glm::vec2 size, const glm::vec2 offset = glm::vec2(0, 0), const glm::vec2 depth = glm::vec2(0, 1));

		/// <summary>
		/// Binds a vertex buffer to the given slot
		/// </summary>
		void IASetVertexBuffer(VertexBuffer& vertBuffer, int slot = 0);

		/// <summary>
		/// Determines how vertices are interpreted by the input assembler
		/// </summary>
		void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

		/// <summary>
		/// Binds an array of buffers starting at the given slot
		/// </summary>
		void IASetVertexBuffers(IDynamicCollection<VertexBuffer>& vertBuffers, int startSlot = 0);

		/// <summary>
		/// Binds an index buffer to the input assembler. Used with DrawIndexed().
		/// </summary>
		void IASetIndexBuffer(IndexBuffer& idxBuf);

		/// <summary>
		/// Assigns given constant buffer to the given slot
		/// </summary>
		void VSSetConstantBuffer(ConstantBuffer& buffer, UINT slot = 0);

		/// <summary>
		/// Binds the given render target to the output merger
		/// </summary>
		void OMSetRenderTarget(ComPtr<ID3D11RenderTargetView>& pRT);

		/// <summary>
		/// Draw indexed, non-instanced primitives
		/// </summary>
		void DrawIndexed(UINT length, UINT start = 0, UINT baseVertexLocation = 0);

	private:
		ComPtr<ID3D11Device> pDev;
		ComPtr<ID3D11DeviceContext> pContext;

	};
}