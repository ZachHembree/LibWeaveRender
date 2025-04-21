#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/BufferBase.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/CtxImm.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

BufferBase::BufferBase() : desc({})
{ }

BufferBase::BufferBase(
	ResourceBindFlags type, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess, 
	Device& dev, 
	const void* data, 
	const uint byteSize
) :
	ResourceBase(dev),
	desc({})
{
	D3D_ASSERT_MSG(byteSize > 0, "Buffer size cannot be 0.");
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
		D3D_CHECK_HR(dev->CreateBuffer(&desc, &resDesc, &pBuf));
	else
		D3D_CHECK_HR(dev->CreateBuffer(&desc, nullptr, &pBuf));
}

ID3D11Buffer* BufferBase::Get() { return pBuf.Get(); }

ID3D11Buffer** const BufferBase::GetAddressOf() { return pBuf.GetAddressOf(); }

ID3D11Resource* BufferBase::GetResource() { return Get(); }

ID3D11Resource** const BufferBase::GetResAddress() { return reinterpret_cast<ID3D11Resource**>(GetAddressOf()); }

uint BufferBase::GetSize() const { return desc.ByteWidth; }

ResourceUsages BufferBase::GetUsage() const { return (ResourceUsages)desc.Usage; }

ResourceBindFlags BufferBase::GetBindFlags() const { return (ResourceBindFlags)desc.BindFlags; }

ResourceAccessFlags BufferBase::GetAccessFlags() const { return (ResourceAccessFlags)desc.CPUAccessFlags; }

void BufferBase::SetData(CtxBase& ctx, const IDynamicArray<byte>& data)
{
	ctx.SetBufferData(*this, data);
}
