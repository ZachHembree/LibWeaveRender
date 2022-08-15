#pragma once
#include "GfxException.hpp"
#include <d3d11.h>
#include <wrl.h>
#include <glm/glm.hpp>
#include "DynamicCollections.hpp"
#include "UniqueObj.hpp"

namespace Replica::D3D11
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class BufferBase;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class SwapChain;
	class InputLayout;
	class VertexShader;
	struct IAElement;

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

	class Device : protected UniqueObjBase
	{
	public:
		class Child;

		using vec2 = glm::vec2;
		using vec4 = glm::vec4;

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
		void ClearRenderTarget(const ComPtr<ID3D11RenderTargetView>& rtView, vec4 color);

		/// <summary>
		/// Binds the given viewport to the rasterizer stage
		/// </summary>
		void RSSetViewport(const vec2 size, const vec2 offset = vec2(0, 0), const vec2 depth = vec2(0, 1));

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
		void IASetVertexBuffers(IDynamicCollection<VertexBuffer>& vertBuffers, int startSlot = 0);

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