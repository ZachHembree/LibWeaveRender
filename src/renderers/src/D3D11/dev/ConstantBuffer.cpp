#include "D3D11/dev/ConstantBuffer.hpp"

using namespace Replica::D3D11;

ConstantBuffer::ConstantBuffer(Device& device,
	size_t size,
	const void* data
) :
	BufferBase(
		ResourceTypes::Constant,
		ResourceUsages::Dynamic,
		ResourceAccessFlags::Write,
		device,
		data,
		(UINT)size
	) 
{ }

void ConstantBuffer::SetData(const void* data, Context & ctx)
{
	UpdateMapUnmap(data, ctx);
}
