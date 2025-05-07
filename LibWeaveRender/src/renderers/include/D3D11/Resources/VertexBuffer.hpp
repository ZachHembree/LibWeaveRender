#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{
	class VertexBuffer : public BufferBase
	{
	public:
		DECL_DEST_MOVE(VertexBuffer);

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
			uint count,
			uint stride,
			uint offset = 0,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		VertexBuffer();

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		uint GetLength() const;

		/// <summary>
		/// Returns the buffer's size in bytes
		/// </summary>
		uint GetStride() const;

		/// <summary>
		/// Returns the number of bytes between the first element between the first element 
		/// in the VB and the first element that will be used
		/// </summary>
		uint GetOffset() const;

	private:
		uint count;
		uint stride;
		uint offset;
	};
}