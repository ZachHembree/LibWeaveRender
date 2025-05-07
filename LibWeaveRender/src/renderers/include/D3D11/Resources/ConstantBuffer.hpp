#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		DECL_DEST_MOVE(ConstantBuffer);

		ConstantBuffer();

		ConstantBuffer(Device& device, size_t size, const void* data = nullptr);

	private:

	};
}