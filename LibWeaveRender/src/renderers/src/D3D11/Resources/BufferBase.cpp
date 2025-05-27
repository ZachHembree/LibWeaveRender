#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/BufferBase.hpp"
#include "D3D11/CtxBase.hpp"
#include "D3D11/CtxImm.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

DEF_MOVE_ONLY(BufferBase);

BufferBase::BufferBase() : desc({})
{ }

BufferBase::BufferBase(
	ResourceBindFlags type, 
	ResourceUsages usage, 
	ResourceAccessFlags cpuAccess, 
	Device& dev, 
	const void* data, 
	const uint byteSize,
	uint miscFlags,
	uint structStride
) :
	ResourceBase(dev),
	desc({
		.byteWidth = byteSize,
		.usage = usage,
		.bindFlags = type,
		.cpuAccessFlags = cpuAccess,
		.miscFlags = miscFlags,
		.structuredStride = structStride
	})
{
	if (byteSize > 0)
	{
		D3D11_SUBRESOURCE_DATA resDesc = {};

		resDesc.pSysMem = data;
		resDesc.SysMemPitch = 0;
		resDesc.SysMemSlicePitch = 0;

		if (data != nullptr)
			D3D_CHECK_HR(dev->CreateBuffer(desc.GetD3DPtr(), &resDesc, &pBuf));
		else
			D3D_CHECK_HR(dev->CreateBuffer(desc.GetD3DPtr(), nullptr, &pBuf));
	}
}

BufferBase::~BufferBase() = default;

void BufferBase::Init() {}

ID3D11Buffer* BufferBase::GetBuffer() { return pBuf.Get(); }

ID3D11Resource* BufferBase::GetResource() const { return pBuf.Get(); }

uint BufferBase::GetByteSize() const { return desc.byteWidth; }

void BufferBase::SetCapacityBytes(uint byteSize)
{
	if (byteSize == desc.byteWidth)
		return;

	desc.byteWidth = byteSize;
	D3D_CHECK_HR(GetDevice()->CreateBuffer(desc.GetD3DPtr(), nullptr, &pBuf));
	Init();
}

uivec3 BufferBase::GetDimensions() const { return uivec3(GetByteSize(), 1, 1); }

ResourceUsages BufferBase::GetUsage() const { return desc.usage; }

ResourceBindFlags BufferBase::GetBindFlags() const { return desc.bindFlags; }

ResourceAccessFlags BufferBase::GetAccessFlags() const { return desc.cpuAccessFlags; }

void BufferBase::SetRawData(CtxBase& ctx, const IDynamicArray<byte>& data)
{
	ctx.SetBufferData(*this, data);
}
