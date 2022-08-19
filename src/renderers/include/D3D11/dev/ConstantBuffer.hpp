#pragma once
#include "D3D11/dev/BufferBase.hpp"

namespace Replica::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		template<typename T>
		ConstantBuffer(
			Device& device,
			const T& data) :
			BufferBase(ResourceTypes::Constant, ResourceUsages::Dynamic, ResourceAccessFlags::Write, device, &data, (UINT)sizeof(T))
		{ }

	private:
		
	};
}