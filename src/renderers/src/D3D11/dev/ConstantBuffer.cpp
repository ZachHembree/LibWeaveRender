#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;

/// <summary>
/// Assigns the constant buffer to the given slot
/// </summary>
void ConstantBuffer::Bind(UINT slot)
{
	pDev->GetContext().Get()->VSSetConstantBuffers(slot, 1, pBuf.GetAddressOf());
}
