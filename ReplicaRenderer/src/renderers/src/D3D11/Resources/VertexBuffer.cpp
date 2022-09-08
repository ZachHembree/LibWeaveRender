#include "D3D11/Resources/VertexBuffer.hpp"
#include "D3D11/Device.hpp"

using namespace Replica::D3D11;

VertexBuffer::VertexBuffer(
	Device& device, 
	const void* data, 
	size_t count, 
	size_t stride,
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count((UINT)count),
	stride((UINT)stride),
	BufferBase(ResourceBindFlags::Vertex, usage, cpuAccess, device, data, (UINT)(stride * count))
{ }

/// <summary>
/// Returns the number of elements in the buffer
/// </summary>
UINT VertexBuffer::GetLength() const { return count; }

/// <summary>
/// Binds the vertex buffer to the given slot
/// </summary>
void VertexBuffer::Bind(Context& ctx, UINT slot, UINT offset)
{
	ctx->IASetVertexBuffers(slot, 1, GetAddressOf(), &stride, &offset);
}
