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
			const Device& device,
			const IDynamicCollection<T>& data,
			BufferUsages usage = BufferUsages::Default, 
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			count((UINT)data.GetLength()),
			stride((UINT)sizeof(T)),
			BufferBase(BufferTypes::Vertex, usage, cpuAccess, device, data)
		{ }

		UINT GetLength() { return count; };

	private:
		const UINT count;

	};
}