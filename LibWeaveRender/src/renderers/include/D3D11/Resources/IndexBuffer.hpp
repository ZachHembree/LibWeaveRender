#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{ 
	class IndexBuffer : public BufferBase
	{
	public:

		IndexBuffer(
			Device& device,
			const IDynamicArray<USHORT>& data,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		IndexBuffer();

		IndexBuffer(IndexBuffer&&);

		IndexBuffer& operator=(IndexBuffer&&);

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		uint GetLength() const;

	private:
		uint count;
	};
}