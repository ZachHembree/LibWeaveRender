#include "pch.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Resources/Texture2DBase.hpp"

using namespace DirectX;
using namespace Weave;
using namespace Weave::D3D11;

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
	desc.Width = dim.x;
	desc.Height = dim.y;
	desc.Format = (DXGI_FORMAT)format;

	desc.MipLevels = mipLevels;
	desc.ArraySize = 1;

	desc.Usage = (D3D11_USAGE)usage;
	desc.BindFlags = (D3D11_BIND_FLAG)bindFlags;
	desc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)accessFlags;
	desc.MiscFlags = 0u;

	// Multisampling
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	if (data != nullptr)
	{ 
		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data;
		initData.SysMemPitch = dim.x * stride;
		D3D_CHECK_HR(GetDevice()->CreateTexture2D(&desc, &initData, &pRes));
	}
	else if (dim.x != 0 && dim.y != 0)
	{
		D3D_CHECK_HR(GetDevice()->CreateTexture2D(&desc, nullptr, &pRes));
	}
}

Texture2DBase::Texture2DBase() : desc({}) {}

uivec3 Texture2DBase::GetDimensions() const { return ivec3(desc.Width, desc.Height, 1u); }

uivec2 Texture2DBase::GetSize() const { return uivec2(desc.Width, desc.Height); }

vec4 Texture2DBase::GetTexelSize() const
{
	uivec2 size = GetSize();
	return vec4(1.0f / vec2(size), size);
}

uint Texture2DBase::GetPixelPitch() const { return pixelStride; }

Formats Texture2DBase::GetFormat() const { return (Formats)desc.Format; }

ResourceUsages Texture2DBase::GetUsage() const { return (ResourceUsages)desc.Usage; }

ResourceBindFlags Texture2DBase::GetBindFlags() const { return (ResourceBindFlags)desc.BindFlags; }

ResourceAccessFlags Texture2DBase::GetAccessFlags() const { return (ResourceAccessFlags)desc.CPUAccessFlags; }

const D3D11_TEXTURE2D_DESC& Texture2DBase::GetDescription() const
{
	return desc;
}

/// <summary>
/// Returns interface to resource
/// </summary>
ID3D11Resource* Texture2DBase::GetResource() { return pRes.Get(); }

void Texture2DBase::LoadImageWIC(wstring_view file, ScratchImage& buffer)
{
	D3D_CHECK_HR(LoadFromWICFile(
		file.data(),
		WIC_FLAGS::WIC_FLAGS_FORCE_RGB,
		nullptr,
		buffer
	));
}

/// <summary>
/// Returns true if the object is valid and has been initialized
/// </summary>
bool Texture2DBase::GetIsValid() const { return pDev != nullptr && pRes.Get() != nullptr; }

ID3D11Resource** const Texture2DBase::GetResAddress() { return reinterpret_cast<ID3D11Resource**>(pRes.GetAddressOf()); }