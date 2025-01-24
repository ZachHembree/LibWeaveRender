#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;

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