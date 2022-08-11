#pragma once
#include "D3D11/BufferBase.hpp"

namespace Replica::D3D11
{
	class VertexBuffer : public BufferBase
	{
	public:
		const UINT stride;

		template<typename T>
		VertexBuffer(
			const Device& device,
			const DynamicArrayBase<T>& data,
			BufferUsages usage = BufferUsages::Default, 
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			stride((UINT)sizeof(T)),
			BufferBase(BufferTypes::Vertex, usage, cpuAccess, device, data)
		{ }

		template<typename T>
		VertexBuffer(
			const Device& device,
			const std::vector<T>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			stride((UINT)sizeof(T)),
			BufferBase(BufferTypes::Vertex, usage, cpuAccess, device, data)
		{ }

	private:

	};
}