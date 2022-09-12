#pragma once
#include "BufferBase.hpp"

namespace Replica::D3D11
{ 
	class IndexBuffer : public BufferBase
	{
	public:

		IndexBuffer(
			Device& device,
			const IDynamicCollection<USHORT>& data,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		IndexBuffer() : count(0) {}

		IndexBuffer(IndexBuffer&&) = default;

		IndexBuffer& operator=(IndexBuffer&&) = default;

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		UINT GetLength() const;

		/// <summary>
		/// Binds an index buffer to the input assembler. Used with DrawIndexed().
		/// </summary>
		void Bind(Context& ctx, UINT offset = 0);

	private:
		UINT count;
	};
}