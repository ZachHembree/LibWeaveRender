#pragma once
#include "BufferBase.hpp"

namespace Replica::D3D11
{
	class VertexBuffer : public BufferBase
	{
	public:
		template<typename T>
		VertexBuffer(
			Device& device,
			const IDynamicArray<T>& data,
			ResourceUsages usage = ResourceUsages::Default, 
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		) :
			VertexBuffer(device, data.GetData(), data.GetLength(), sizeof(T), usage, cpuAccess)
		{ }

		VertexBuffer(
			Device& device,
			const void* data,
			size_t count,
			size_t stride,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		VertexBuffer();

		VertexBuffer(VertexBuffer&&);

		VertexBuffer& operator=(VertexBuffer&&);

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		UINT GetLength() const;

		UINT GetStride() const { return stride; }

		/// <summary>
		/// Binds the vertex buffer to the given slot
		/// </summary>
		void Bind(Context& ctx, UINT slot = 0u, UINT offset = 0u);

	private:
		UINT count;
		UINT stride;
	};
}