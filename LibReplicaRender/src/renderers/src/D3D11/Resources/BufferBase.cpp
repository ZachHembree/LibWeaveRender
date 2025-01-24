#include "pch.hpp"
#include <d3d11.h>
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;

BufferBase::BufferBase() : desc({})
{ }

BufferBase::BufferBase(
	ResourceBindFlags type, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess, 
	Device& dev, 
	const void* data, 
	const UINT byteSize
) :
	ResourceBase(dev),
	desc({})
{
	GFX_ASSERT(byteSize > 0, "Buffer size cannot be 0.");
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
		(dev->CreateBuffer(&desc, &resDesc, &pBuf));
	}
	else
	{
		(dev->CreateBuffer(&desc, nullptr, &pBuf));
	}
}

/// <summary>
/// Returns the size of the buffer in bytes
/// </summary>
ID3D11Buffer* BufferBase::Get() { return pBuf.Get(); }

ID3D11Buffer** const BufferBase::GetAddressOf() { return pBuf.GetAddressOf(); }

ID3D11Resource* BufferBase::GetResource() { return Get(); }

ID3D11Resource** const BufferBase::GetResAddress() { return reinterpret_cast<ID3D11Resource**>(GetAddressOf()); }

UINT BufferBase::GetSize() const { return desc.ByteWidth; }

ResourceUsages BufferBase::GetUsage() const { return (ResourceUsages)desc.Usage; }

ResourceBindFlags BufferBase::GetBindFlags() const { return (ResourceBindFlags)desc.BindFlags; }

ResourceAccessFlags BufferBase::GetAccessFlags() const { return (ResourceAccessFlags)desc.CPUAccessFlags; }

void BufferBase::SetData(Context& ctx, const void* data)
{
	if (GetSize() > 0)
	{
		GFX_ASSERT(GetUsage() != ResourceUsages::Immutable, "Cannot update Buffers without write access.");

		if (GetUsage() == ResourceUsages::Dynamic)
		{
			UpdateMapUnmap(ctx, data);
		}
		else
		{
			UpdateSubresource(ctx, data);
		}
	}
}

void BufferBase::UpdateSubresource(Context& ctx, const void* data)
{
	ctx->UpdateSubresource( Get(), 0, nullptr, data, GetSize(), GetSize());
}

void BufferBase::UpdateMapUnmap(Context& ctx, const void* data)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_THROW_FAILED(ctx->Map(
		Get(),
		0u,
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	memcpy(msr.pData, data, GetSize());
	ctx->Unmap(Get(), 0u);
}