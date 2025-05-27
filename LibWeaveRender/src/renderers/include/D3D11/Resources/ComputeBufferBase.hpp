#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{
	class ComputeBufferBase : public BufferBase
	{
	public:
		DECL_DEST_MOVE(ComputeBufferBase);

		/// <summary>
		/// Returns the number of elements in the compute buffer
		/// </summary>
		uint GetLength() const;

		/// <summary>
		/// Returns the maximum number of elements that can be stored in the compute buffer
		/// </summary>
		uint GetCapacity() const;

		/// <summary>
		/// Sets the maximum number of elements that cna be stored in the compute buffer. Will 
		/// allocate a new buffer and destroy previous contents if the size changes.
		/// </summary>
		template<typename T>
		void SetCapacity(uint count) { SetCapacity(count, (uint)sizeof(T)); }

		/// <summary>
		/// Sets the buffer to the given size. If capacity is increased, buffer contents will be 
		/// invalidated. If size is less than capacity, then contents outside the active range are 
		/// undefined.
		/// </summary>
		template<typename T>
		void Resize(uint count) { Resize(count, (uint)sizeof(T)); }

		/// <summary>
		/// Sets the maximum number of elements that cna be stored in the compute buffer. Will 
		/// allocate a new buffer and destroy previous contents if the size changes.
		/// </summary>
		void SetCapacity(uint count, uint typeSize = 0);

		/// <summary>
		/// Sets the buffer to the given size. If capacity is increased, buffer contents will be 
		/// invalidated. If size is less than capacity, then contents outside the active range are 
		/// undefined.
		/// </summary>
		void Resize(uint count, uint typeSize = 0);

		/// <summary>
		/// Returns the size of individual elements in the buffer
		/// </summary>
		uint GetStructureStride() const;

		/// <summary>
		/// Returns the dimensions of the underlying buffer. Non-applicable dimensions are 
		/// always set to 1. For 1D buffers, Y == 1 and Z == 1.
		/// </summary>
		uivec3 GetDimensions() const override;

		/// <summary>
		/// Writes the given data set to the compute buffer
		/// </summary>
		template<typename T>
		void SetData(CtxBase& ctx, const IDynamicArray<T>& data)
		{
			SetRawData(ctx, Span<byte>((byte*)data.GetData(), GetArrSize(data)));
		}

	protected:
		uint count;

		ComputeBufferBase();

		ComputeBufferBase(
			ResourceBindFlags type,
			ResourceUsages usage,
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const uint typeSize = 0,
			const uint count = 0, 
			const void* data = nullptr
		);
	};
}