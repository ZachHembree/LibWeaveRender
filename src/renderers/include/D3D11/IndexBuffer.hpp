#pragma once
#include "D3D11/BufferBase.hpp"

namespace Replica::D3D11
{ 
	class IndexBuffer : public BufferBase
	{
	public:

		IndexBuffer(
			const Device& device,
			const DynamicArrayBase<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			BufferBase(BufferTypes::Index, usage, cpuAccess, device, data)
		{ }

		IndexBuffer(
			const Device& device,
			const std::vector<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			BufferBase(BufferTypes::Index, usage, cpuAccess, device, data)
		{ }

	private:

	};
}