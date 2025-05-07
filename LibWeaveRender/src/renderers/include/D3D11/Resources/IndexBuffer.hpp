#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{ 
	/// <summary>
	/// Contains a list of indices for arranging a set of vertices into a mesh. 
	/// Indices can be 16 or 32 bit unsigned integers.
	/// </summary>
	class IndexBuffer : public BufferBase
	{
	public:
		MAKE_MOVE_ONLY(IndexBuffer);

		IndexBuffer(
			Device& device,
			const IDynamicArray<ushort>& data,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		IndexBuffer(
			Device& device,
			const IDynamicArray<uint>& data,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceAccessFlags cpuAccess = ResourceAccessFlags::None
		);

		IndexBuffer();

		/// <summary>
		/// Returns the number of elements in the buffer
		/// </summary>
		uint GetLength() const;

		/// <summary>
		/// Returns the format of the indices stored by the buffer, either 32 bit
		/// or 16 bit unsigned integers.
		/// </summary>
		Formats GetFormat() const;

	private:
		uint count;
		Formats format;
	};
}