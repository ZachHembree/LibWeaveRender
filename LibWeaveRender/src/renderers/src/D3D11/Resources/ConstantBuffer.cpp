#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

ConstantBuffer::ConstantBuffer() { }

ConstantBuffer::ConstantBuffer(Device& device,
	size_t size,
	const void* data
) :
	BufferBase(
		ResourceBindFlags::Constant,
		ResourceUsages::Dynamic,
		ResourceAccessFlags::Write,
		device,
		data,
		(uint)GetAlignedByteSize(size, 16)
	) 
{ }