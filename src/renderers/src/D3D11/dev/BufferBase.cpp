#include "D3D11/D3DUtils.hpp"
#include <d3d11.h>
#include "D3D11/dev/BufferBase.hpp"
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/Context.hpp"

using namespace Replica::D3D11;

BufferBase::BufferBase() :
	byteSize(0),
	type(ResourceTypes::Constant),
	usage(ResourceUsages::Immutable),
	cpuAccess(ResourceAccessFlags::None)
{ }

BufferBase::BufferBase(
	ResourceTypes type, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess, 
	Device& dev, 
	const void* data, 
	const UINT byteSize
) :
	ResourceBase(&dev),
	type(type),
	usage(usage),
	cpuAccess(cpuAccess),
	byteSize(byteSize)
{
	CreateBuffer(data, byteSize, dev.Get());
}

BufferBase::BufferBase(BufferBase&& other) noexcept :
	ResourceBase(other.pDev),
	type(other.type),
	usage(other.usage),
	cpuAccess(other.cpuAccess),
	pBuf(std::move(other.pBuf)),
	byteSize(other.byteSize)
{ }

BufferBase& BufferBase::operator=(BufferBase&& other) noexcept
{
	this->pDev = pDev;
	this->type = other.type;
	this->usage = other.usage;
	this->cpuAccess = other.cpuAccess;
	pBuf = std::move(other.pBuf);
	this->byteSize = other.byteSize;

	return *this;
}

void BufferBase::CreateBuffer(const void* data, const UINT byteSize, ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc = {};
	D3D11_SUBRESOURCE_DATA resDesc = {};

	desc.Usage = (D3D11_USAGE)usage;
	desc.BindFlags = (UINT)type;
	desc.ByteWidth = byteSize;
	desc.CPUAccessFlags = (UINT)cpuAccess;
	desc.MiscFlags = 0;

	resDesc.pSysMem = data;
	resDesc.SysMemPitch = 0;
	resDesc.SysMemSlicePitch = 0;

	if (data != nullptr)
	{
		GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, &resDesc, &pBuf));
	}
	else
	{
		GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, nullptr, &pBuf));
	}
}

void BufferBase::UpdateMapUnmap(const void* data, Context& ctx)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_THROW_FAILED(ctx.Get()->Map(
		Get(),
		0u,
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	memcpy(msr.pData, data, byteSize);
	ctx.Get()->Unmap(Get(), 0u);
}