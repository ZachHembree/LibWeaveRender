#include "D3D11/dev/VertexBuffer.hpp"

using namespace Replica::D3D11;

/// <summary>
/// Returns the number of elements in the buffer
/// </summary>
UINT VertexBuffer::GetLength() const { return count; }

/// <summary>
/// Binds the vertex buffer to the given slot
/// </summary>
void VertexBuffer::Bind(UINT slot, UINT offset)
{
	pDev->GetContext()->IASetVertexBuffers(slot, 1, pBuf.GetAddressOf(), &stride, &offset);
}
