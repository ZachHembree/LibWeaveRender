#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		ConstantBuffer();

		ConstantBuffer(Device& device, size_t size, const void* data = nullptr);

		ConstantBuffer(ConstantBuffer&& other) = default;

		ConstantBuffer& operator=(ConstantBuffer&& other) = default;

	private:

	};
}