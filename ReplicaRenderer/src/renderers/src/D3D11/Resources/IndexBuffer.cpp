#include "D3D11/Resources/IndexBuffer.hpp"
#include "D3D11/Resources/Formats.hpp"
#include "D3D11/Device.hpp"

using namespace Replica::D3D11;

IndexBuffer::IndexBuffer(Device& device, 
	const IDynamicCollection<USHORT>& data, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess
) :
	count((UINT)data.GetLength()),
	BufferBase(ResourceTypes::Index, usage, cpuAccess, device, data)
{ }

/// <summary>
/// Returns the number of elements in the buffer
/// </summary>
UINT IndexBuffer::GetLength() const { return count; }

/// <summary>
/// Binds an index buffer to the input assembler. Used with DrawIndexed().
/// </summary>
void IndexBuffer::Bind(Context& ctx, UINT offset)
{
	ctx.Get()->IASetIndexBuffer(Get(), (DXGI_FORMAT)Formats::R16_UINT, offset);
}
