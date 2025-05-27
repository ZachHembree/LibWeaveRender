#include "pch.hpp"
#include "D3D11/Resources/ComputeBufferBase.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(ComputeBufferBase);

ComputeBufferBase::ComputeBufferBase() :
	count(0)
{ }

ComputeBufferBase::ComputeBufferBase(
	ResourceBindFlags type,
	ResourceUsages usage,
	ResourceAccessFlags cpuAccess,
	Device& device,
	uint typeSize,
	uint count,
	const void* data
) :
	BufferBase(
		type,
		usage,
		cpuAccess,
		device,
		data,
		typeSize * count,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
		typeSize),
	count(count)
{ }

uint ComputeBufferBase::GetLength() const { return count; }

uint ComputeBufferBase::GetCapacity() const { return desc.byteWidth / desc.structuredStride; }

uint ComputeBufferBase::GetByteSize() const { return count * desc.structuredStride; }

uint ComputeBufferBase::GetCapacityBytes() const { return desc.byteWidth; }

void ComputeBufferBase::SetCapacity(uint count, uint typeSize)
{
	if (typeSize == 0)
		typeSize = desc.structuredStride;

	desc.structuredStride = typeSize;
	this->count = count;
	SetByteSize(count * typeSize);
}

void ComputeBufferBase::Resize(uint count, uint typeSize) 
{
	if (typeSize == 0)
		typeSize = desc.structuredStride;

	desc.structuredStride = typeSize;

	if (count > this->count)
	{
		this->count = count;
		SetByteSize(count * typeSize);
	}
	else
		this->count = count;
}

uint ComputeBufferBase::GetStructureStride() const { return desc.structuredStride; }

uivec3 ComputeBufferBase::GetDimensions() const { return uivec3(GetLength(), 1u, 1u); }
