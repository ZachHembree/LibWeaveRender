#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/IndexBuffer.hpp"

using namespace Replica::D3D11;

IndexBuffer::IndexBuffer(Device& device, 
	const IDynamicArray<USHORT>& data, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count((UINT)data.GetLength()),
	BufferBase(ResourceBindFlags::Index, usage, cpuAccess, device, data)
{ }

IndexBuffer::IndexBuffer() : count(0) {}

IndexBuffer::IndexBuffer(IndexBuffer&&) = default;

IndexBuffer& IndexBuffer::operator=(IndexBuffer&&) = default;

/// <summary>
/// Returns the number of elements in the buffer
/// </summary>
UINT IndexBuffer::GetLength() const { return count; }

/// <summary>
/// Binds an index buffer to the input assembler. Used with DrawIndexed().
/// </summary>
void IndexBuffer::Bind(Context& ctx, UINT offset)
{
	ctx->IASetIndexBuffer(Get(), (DXGI_FORMAT)Formats::R16_UINT, offset);
}
