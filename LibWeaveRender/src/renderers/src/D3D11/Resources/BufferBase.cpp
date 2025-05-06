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
	desc({
		.byteWidth= byteSize,
		.usage = usage,
		.bindFlags = type,
		.cpuAccessFlags = cpuAccess
	})
{
	D3D_ASSERT_MSG(byteSize > 0, "Buffer size cannot be 0.");
	D3D11_SUBRESOURCE_DATA resDesc = {};

	resDesc.pSysMem = data;
	resDesc.SysMemPitch = 0;
	resDesc.SysMemSlicePitch = 0;

	if (data != nullptr)
		D3D_CHECK_HR(dev->CreateBuffer(desc.GetD3DPtr(), &resDesc, &pBuf));
	else
		D3D_CHECK_HR(dev->CreateBuffer(desc.GetD3DPtr(), nullptr, &pBuf));
}

BufferBase::BufferBase(BufferBase&&) noexcept = default;

BufferBase& BufferBase::operator=(BufferBase&&) noexcept = default;

BufferBase::~BufferBase() = default;

ID3D11Buffer* BufferBase::Get() { return pBuf.Get(); }

ID3D11Buffer** const BufferBase::GetAddressOf() { return pBuf.GetAddressOf(); }

ID3D11Resource* BufferBase::GetResource() { return Get(); }

ID3D11Resource** const BufferBase::GetResAddress() { return reinterpret_cast<ID3D11Resource**>(GetAddressOf()); }

uint BufferBase::GetSize() const { return desc.byteWidth; }

ResourceUsages BufferBase::GetUsage() const { return desc.usage; }

ResourceBindFlags BufferBase::GetBindFlags() const { return desc.bindFlags; }

ResourceAccessFlags BufferBase::GetAccessFlags() const { return desc.cpuAccessFlags; }

void BufferBase::SetData(CtxBase& ctx, const IDynamicArray<byte>& data)
{
	ctx.SetBufferData(*this, data);
}
