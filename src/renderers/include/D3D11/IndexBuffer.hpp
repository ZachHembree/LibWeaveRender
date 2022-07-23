#pragma once
#include "BufferBase.hpp"

namespace Replica::D3D11
{ 
	class IndexBuffer : public BufferBase
	{
	public:

		IndexBuffer(
			const Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
			const DynamicArrayBase<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			BufferBase(BufferTypes::Index, usage, cpuAccess, pDevice, data)
		{ }

		IndexBuffer(
			const Microsoft::WRL::ComPtr<ID3D11Device>& pDevice,
			const std::vector<USHORT>& data,
			BufferUsages usage = BufferUsages::Default,
			BufferAccessFlags cpuAccess = BufferAccessFlags::None) :
			BufferBase(BufferTypes::Index, usage, cpuAccess, pDevice, data)
		{ }

	private:

	};
}