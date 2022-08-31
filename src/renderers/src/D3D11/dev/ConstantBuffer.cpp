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
		(uint)size
	) 
{ }

ConstantBuffer::ConstantBuffer(ConstantBuffer&& other) noexcept :
	BufferBase(std::move(other))
{ }

ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer&& other) noexcept
{
	return static_cast<ConstantBuffer&>(BufferBase::operator=(std::move(other)));
}

void ConstantBuffer::SetData(const void* data, Context & ctx)
{
	UpdateMapUnmap(data, ctx);
}
