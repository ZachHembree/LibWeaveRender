#pragma once
#include "ComputeBufferBase.hpp"

namespace Weave::D3D11
{
	class StagingComputeBuffer : public ComputeBufferBase
	{
	public:
		DECL_DEST_MOVE(StagingComputeBuffer);

		StagingComputeBuffer();

		StagingComputeBuffer(Device& device);

		StagingComputeBuffer(Device& device, const uint count, const uint typeSize, const void* data = nullptr);

		/// <summary>
		/// Writes the given data set to the compute buffer
		/// </summary>
		template<typename T>
		void SetData(CtxBase& ctx, const IDynamicArray<T>& data)
		{
			SetRawData(ctx, Span<byte>((byte*)data.GetData(), GetArrSize(data)));
		}

	private:

	};
}