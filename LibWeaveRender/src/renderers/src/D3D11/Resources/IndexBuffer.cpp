#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/IndexBuffer.hpp"

using namespace Weave::D3D11;

IndexBuffer::IndexBuffer(Device& device, 
	const IDynamicArray<USHORT>& data, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count((uint)data.GetLength()),
	BufferBase(ResourceBindFlags::Index, usage, cpuAccess, device, data)
{ }

IndexBuffer::IndexBuffer() : count(0) {}

IndexBuffer::IndexBuffer(IndexBuffer&&) = default;

IndexBuffer& IndexBuffer::operator=(IndexBuffer&&) = default;

/// <summary>
/// Returns the number of elements in the buffer
/// </summary>
uint IndexBuffer::GetLength() const { return count; }
