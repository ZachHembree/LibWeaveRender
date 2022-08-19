#pragma once
#include "D3D11/dev/BufferBase.hpp"

namespace Replica::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		ConstantBuffer(
			Device& device, 
			size_t size, 
			const void* data = nullptr
		);

		template<typename T>
		ConstantBuffer(
			Device& device,
			const T& data
		) :
		ConstantBuffer(
			device,
			sizeof(T),
			&data
		) { }

		void SetData(const void* data, Context& ctx);

	private:
		
	};
}