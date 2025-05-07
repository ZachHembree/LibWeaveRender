#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/VertexBuffer.hpp"

using namespace Weave::D3D11;

DEF_DEST_MOVE(VertexBuffer);

VertexBuffer::VertexBuffer(
	Device& device, 
	const void* data, 
	uint count,
	uint stride,
	uint offset,
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count(count),
	stride(stride),
	offset(offset),
	BufferBase(ResourceBindFlags::Vertex, usage, cpuAccess, device, data, (stride * count))
{ }

VertexBuffer::VertexBuffer() : count(0), stride(0), offset(0) {}

uint VertexBuffer::GetLength() const { return count; }

uint VertexBuffer::GetStride() const { return stride; }

uint VertexBuffer::GetOffset() const { return offset; }
