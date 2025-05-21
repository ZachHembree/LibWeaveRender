#include "pch.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/CtxImm.hpp"
#include "D3D11/Resources/Texture2DBase.hpp"

using namespace DirectX;
using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(Texture2DBase);

Texture2DBase::Texture2DBase(
	Device& dev,
	uivec2 dim,
	Formats format,
	ResourceUsages usage,
	ResourceBindFlags bindFlags,
	ResourceAccessFlags accessFlags,
	uint mipLevels,
	uint arraySize,
	void* data,
	uint stride
) :
	ResourceBase(dev),
	pixelStride(stride)
{
	desc = {};
	desc.size = dim;
	desc.format = format;

	desc.mipLevels = mipLevels;
	desc.arraySize = 1;

	desc.usage = usage;
	desc.bindFlags = bindFlags;
	desc.cpuAccessFlags = accessFlags;
	desc.miscFlags = 0u;

	// Multisampling
	desc.sampleDesc.count = 1;
	desc.sampleDesc.quality = 0;

	if (dim.x != 0 && dim.y != 0)
	{
		if (data != nullptr)
		{
			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = data;
			initData.SysMemPitch = dim.x * stride;
			D3D_CHECK_HR(GetDevice()->CreateTexture2D(desc.GetD3DPtr(), &initData, &pRes));
		}
		else
		{
			D3D_CHECK_HR(GetDevice()->CreateTexture2D(desc.GetD3DPtr(), nullptr, &pRes));
		}
	}
}

Texture2DBase::Texture2DBase() : desc({}) {}

void Texture2DBase::Reset(uivec2 dim, Formats format)
{
	if (dim != uivec2(-1))
		desc.size = dim;

	if (format != Formats::UNKNOWN)
		desc.format = (DXGI_FORMAT)format;

	D3D_CHECK_HR(GetDevice()->CreateTexture2D(desc.GetD3DPtr(), nullptr, &pRes));
	Init();
}

uivec3 Texture2DBase::GetDimensions() const { return uivec3(desc.size.x, desc.size.y, 1u); }

uivec2 Texture2DBase::GetSize() const { return desc.size; }

vec4 Texture2DBase::GetTexelSize() const
{
	uivec2 size = GetSize();
	return vec4(1.0f / vec2(size), size);
}

uint Texture2DBase::GetPixelPitch() const { return pixelStride; }

Formats Texture2DBase::GetFormat() const { return desc.format; }

ResourceUsages Texture2DBase::GetUsage() const { return desc.usage; }

ResourceBindFlags Texture2DBase::GetBindFlags() const { return desc.bindFlags; }

ResourceAccessFlags Texture2DBase::GetAccessFlags() const { return desc.cpuAccessFlags; }

const Texture2DDesc& Texture2DBase::GetDescription() const { return desc; }

/// <summary>
/// Returns interface to resource
/// </summary>
ID3D11Resource* Texture2DBase::GetResource() const { return pRes.Get(); }

void Texture2DBase::SetTextureData(CtxBase& ctx, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim)
{
	if (srcDim != GetSize())
		Reset(srcDim);

	ctx.SetTextureData(*this, src, pixStride, srcDim);
	this->pixelStride = pixStride;
}

void Texture2DBase::LoadImageWIC(wstring_view file, ScratchImage& buffer)
{
	D3D_CHECK_HR(LoadFromWICFile(
		file.data(),
		WIC_FLAGS::WIC_FLAGS_FORCE_RGB,
		nullptr,
		buffer
	));
}

bool Texture2DBase::GetIsValid() const { return pDev != nullptr && pRes.Get() != nullptr; }

ID3D11Resource* const* Texture2DBase::GetResAddress() const { return reinterpret_cast<ID3D11Resource* const*>(pRes.GetAddressOf()); }