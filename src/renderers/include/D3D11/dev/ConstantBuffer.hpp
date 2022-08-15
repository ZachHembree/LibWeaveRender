#pragma once
#include "D3D11/dev/BufferBase.hpp"

namespace Replica::D3D11
{
	class ConstantBuffer : public BufferBase
	{
	public:
		template<typename T>
		ConstantBuffer(
			const Device& device,
			const T& data) :
			BufferBase(BufferTypes::Constant, BufferUsages::Dynamic, BufferAccessFlags::Write, device, &data, (UINT)sizeof(T))
		{ }

		/// <summary>
		/// Assigns the constant buffer to the given slot
		/// </summary>
		void Bind(UINT slot = 0);

	private:
		
	};
}