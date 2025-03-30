#pragma once
#include "BufferBase.hpp"

namespace Replica::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		template<typename T>
		ConstantBuffer(Device& device, const T& data) :
			ConstantBuffer(device, sizeof(T), &data)
		{ }

		ConstantBuffer();

		ConstantBuffer(Device& device, size_t size, const void* data = nullptr);

		ConstantBuffer(ConstantBuffer&& other) = default;

		ConstantBuffer& operator=(ConstantBuffer&& other) = default;

	private:

	};
}