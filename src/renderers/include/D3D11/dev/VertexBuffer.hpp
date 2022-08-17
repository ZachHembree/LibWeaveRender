#pragma once
#include "D3D11/dev/BufferBase.hpp"

namespace Replica::D3D11
{
	class VertexBuffer : public BufferBase
	{
	public:
		const UINT stride;

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

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		UINT GetLength() const;

		/// <summary>
		/// Binds the vertex buffer to the given slot
		/// </summary>
		void Bind(Context& ctx, UINT slot = 0u, UINT offset = 0u);

	private:
		const UINT count;

	};
}