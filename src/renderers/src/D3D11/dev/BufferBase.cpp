#include "GfxException.hpp"
#include <d3d11.h>
#include "D3D11/dev/BufferBase.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;

Replica::D3D11::BufferBase::BufferBase(
	BufferTypes type, 
	BufferUsages usage, 
	BufferAccessFlags cpuAccess, 
	const Device& device, 
	const void* data, 
	const UINT byteSize
) :
	type(type),
	usage(usage),
	cpuAccess(cpuAccess)
{
	CreateBuffer(data, byteSize, device.Get());
}

void Replica::D3D11::BufferBase::CreateBuffer(const void* data, const UINT byteSize, ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA resDesc;

	GetBufferDesc(data, byteSize, desc, resDesc);
	GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, &resDesc, &pBuf));
}

void Replica::D3D11::BufferBase::GetBufferDesc(const void* data, const UINT byteSize, D3D11_BUFFER_DESC& desc, D3D11_SUBRESOURCE_DATA& resDesc)
{
	desc = {};
	desc.Usage = (D3D11_USAGE)usage;
	desc.BindFlags = (UINT)type;
	desc.ByteWidth = byteSize;
	desc.CPUAccessFlags = (UINT)cpuAccess;
	desc.MiscFlags = 0;

	resDesc.pSysMem = data;
	resDesc.SysMemPitch = 0;
	resDesc.SysMemSlicePitch = 0;
}
