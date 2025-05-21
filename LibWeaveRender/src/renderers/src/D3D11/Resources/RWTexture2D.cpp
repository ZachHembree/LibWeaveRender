#include "pch.hpp"
#include <DirectXTex.h>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/CtxImm.hpp"
#include "D3D11/Resources/RWTexture2D.hpp"

using namespace DirectX;
using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(RWTexture2D);

RWTexture2D::RWTexture2D() :
	renderScale(1),
	renderOffset(0)
{ }

RWTexture2D::RWTexture2D(
	Device& dev,
	uivec2 dim,
	Formats format,
	ResourceUsages usage,
	ResourceBindFlags bindFlags,
	ResourceAccessFlags accessFlags,
	uint mipLevels,
	void* data,
	uint stride
) :
	ResizeableTexture2D(
		dev,
		dim,
		format,
		usage,
		bindFlags,
		accessFlags,
		mipLevels,
		data,
		stride
	),
	renderScale(1),
	renderOffset(0)
{
	Init();
}

RWTexture2D::RWTexture2D(Device& dev,
	uivec2 dim,
	void* data,
	uint stride,
	Formats format,
	uint mipLevels
) :
	RWTexture2D(
		dev,
		dim,
		format,
		ResourceUsages::Default,
		ResourceBindFlags::RWTexture,
		ResourceAccessFlags::None,
		mipLevels, 
		data, stride
	)
{ }

RWTexture2D::RWTexture2D(
	Device& dev,
	Formats format,
	uivec2 dim,
	uint mipLevels
) :
	RWTexture2D(
		dev,
		dim,
		format,
		ResourceUsages::Default,
		ResourceBindFlags::RWTexture,
		ResourceAccessFlags::None,
		1u,
		nullptr, 0u
	)
{ }

void RWTexture2D::Init()
{
	if (pRes.Get() != nullptr)
	{
		// SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = (DXGI_FORMAT)desc.format;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.Texture2D.MipLevels = desc.mipLevels;

		D3D_CHECK_HR(GetDevice()->CreateShaderResourceView(pRes.Get(), &vDesc, &pSRV));

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = (DXGI_FORMAT)desc.format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		D3D_CHECK_HR(GetDevice()->CreateUnorderedAccessView(pRes.Get(), &uavDesc, &pUAV));

		// RTV
		D3D_CHECK_HR(GetDevice()->CreateRenderTargetView(pRes.Get(), nullptr, &pRTV));
	}
}

/// <summary>
/// Returns pointer to UAV interface
/// </summary>
ID3D11UnorderedAccessView* RWTexture2D::GetUAV() { return pUAV.Get(); }

/// <summary>
/// Returns pointer to UAV pointer field
/// </summary>
ID3D11UnorderedAccessView* const* RWTexture2D::GetAddressUAV() { return pUAV.GetAddressOf(); }

ID3D11RenderTargetView* RWTexture2D::GetRTV() { return pRTV.Get(); }

ID3D11RenderTargetView* const* RWTexture2D::GetAddressRTV() { return pRTV.GetAddressOf(); }

void RWTexture2D::Clear(CtxBase& ctx, vec4 color)
{
	ctx.ClearRenderTarget(*this);
}

void RWTexture2D::SetTextureData(CtxBase& ctx, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim)
{
	if (AnyTrue(srcDim > GetSize()))
		Reset(srcDim);

	SetRenderSize(srcDim);
	ctx.SetTextureData(*this, src, pixStride, srcDim);
	this->pixelStride = pixStride;
}

RWTexture2D RWTexture2D::FromImageWIC(Device& dev, wstring_view file)
{
	ScratchImage buf;
	LoadImageWIC(file, buf);
	const Image& img = *buf.GetImage(0, 0, 0);

	return RWTexture2D(dev,
		uivec2(img.width, img.height),
		img.pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM
	);
}
