#pragma once
#include "D3D11/dev/BufferBase.hpp"

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
			count((UINT)data.GetLength()),
			BufferBase(BufferTypes::Index, usage, cpuAccess, device, data)
		{ }

		IndexBuffer(
			const Device& device,
			const std::vector<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			count((UINT)data.size()),
			BufferBase(BufferTypes::Index, usage, cpuAccess, device, data)
		{ }

		UINT GetLength() { return count; }

	private:
		UINT count;
	};
}