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
	const uint typeSize,
	const uint count,
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

void ComputeBufferBase::SetCapacity(uint count, uint typeSize)
{
	if (typeSize == 0)
		typeSize = desc.structuredStride;

	SetCapacityBytes(count * typeSize);
	this->count = count;
	desc.structuredStride = typeSize;
}

void ComputeBufferBase::Resize(uint count, uint typeSize) 
{
	if (typeSize == 0)
		typeSize = desc.structuredStride;

	if (count > this->count)
		SetCapacity(count, typeSize);
}

uint ComputeBufferBase::GetStructureStride() const { return desc.structuredStride; }

uivec3 ComputeBufferBase::GetDimensions() const { return uivec3(GetLength(), 1u, 1u); }
