#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/IndexBuffer.hpp"

using namespace Weave::D3D11;

IndexBuffer::IndexBuffer(Device& device, 
	const IDynamicArray<ushort>& data,
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count((uint)data.GetLength()),
	BufferBase(ResourceBindFlags::Index, usage, cpuAccess, device, data),
	format(Formats::R16_UINT)
{ }

IndexBuffer::IndexBuffer(Device& device,
	const IDynamicArray<uint>& data,
	ResourceUsages usage,
	ResourceAccessFlags cpuAccess
) :
	count((uint)data.GetLength()),
	BufferBase(ResourceBindFlags::Index, usage, cpuAccess, device, data),
	format(Formats::R32_UINT)
{ }

IndexBuffer::IndexBuffer() : count(0), format(Formats::UNKNOWN) {}

uint IndexBuffer::GetLength() const { return count; }

Formats IndexBuffer::GetFormat() const { return format; }
