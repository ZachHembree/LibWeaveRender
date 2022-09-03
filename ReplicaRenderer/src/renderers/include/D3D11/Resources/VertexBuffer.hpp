#pragma once
#include "D3D11/Resources/BufferBase.hpp"

namespace Replica::D3D11
{
	class VertexBuffer : public BufferBase
	{
	public:
		template<typename T>
		VertexBuffer(
			Device& device,
			const IDynamicCollection<T>& data,
			ResourceUsages usage = ResourceUsages::Default, 
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None) :
			count((UINT)data.GetLength()),
			stride((UINT)sizeof(T)),
			BufferBase(ResourceTypes::Vertex, usage, cpuAccess, device, data)
		{ }

		VertexBuffer() : count(0), stride(0) { }

		VertexBuffer(VertexBuffer&&) = default;

		VertexBuffer& operator=(VertexBuffer&&) = default;

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