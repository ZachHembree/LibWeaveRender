#pragma once
#include "D3D11/dev/BufferBase.hpp"

namespace Replica::D3D11
{ 
	class IndexBuffer : public BufferBase
	{
	public:

		IndexBuffer(
			const Device& device,
			const IDynamicCollection<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None
		);

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		UINT GetLength() const;

		/// <summary>
		/// Binds an index buffer to the input assembler. Used with DrawIndexed().
		/// </summary>
		void Bind(UINT offset = 0);

	private:
		UINT count;
	};
}