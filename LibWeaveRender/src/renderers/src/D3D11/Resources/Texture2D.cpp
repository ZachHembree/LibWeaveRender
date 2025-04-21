#include "pch.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Resources/Texture2D.hpp"

using namespace DirectX;
using namespace Weave::D3D11;

Texture2D::Texture2D() 
{ }

Texture2D::Texture2D(
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
	Texture2DBase(
		dev,
		dim,
		format,
		usage,
		bindFlags,
		accessFlags,
		mipLevels, 1u,
		data, stride
	)
{
	if (pRes.Get() != nullptr)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = desc.Format;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.Texture2D.MipLevels = desc.MipLevels;

		D3D_CHECK_HR(dev->CreateShaderResourceView(pRes.Get(), &vDesc, &pSRV));
	}
}

Texture2D::Texture2D(
	Device& dev,
	uivec2 dim,
	void* data,
	uint stride,
	Formats format,
	uint mipLevels,
	bool isReadonly
) :
	Texture2D(
		dev,
		dim,
		format,
		isReadonly ? ResourceUsages::Immutable : ResourceUsages::Default,
		ResourceBindFlags::ShaderResource,
		ResourceAccessFlags::None,
		mipLevels,
		data, stride
	)
{ }

Texture2D::Texture2D(
	Device& dev,
	Formats format,
	uivec2 dim,
	uint mipLevels,
	bool isDynamic
) :
	Texture2D(
		dev,
		dim,
		format,
		isDynamic ? ResourceUsages::Dynamic : ResourceUsages::Default,
		ResourceBindFlags::ShaderResource,
		isDynamic ? ResourceAccessFlags::Write : ResourceAccessFlags::None,
		mipLevels,
		nullptr, 0u
	)
{ }

/// <summary>
/// Returns true if the texture is immutable
/// </summary>
bool Texture2D::GetIsReadOnly() const
{
	return GetUsage() == ResourceUsages::Immutable;
}

ID3D11ShaderResourceView* Texture2D::GetSRV() { return pSRV.Get(); }

ID3D11ShaderResourceView** const Texture2D::GetSRVAddress() { return pSRV.GetAddressOf(); }

void Texture2D::SetTextureWIC(ContextBase& ctx, wstring_view file, ScratchImage& buffer)
{
	LoadImageWIC(file, buffer);
	const Image& img = *buffer.GetImage(0, 0, 0);
	const uint pixStride = 4 * sizeof(byte);
	const uivec2 dim(img.width, img.height);
	const uint totalBytes = pixStride * dim.x * dim.y;
	Span srcBytes(img.pixels, totalBytes);

	SetTextureData(ctx, srcBytes, pixStride, dim);
}

void Texture2D::SetTextureData(ContextBase& ctx, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim)
{
	if (srcDim == GetSize())
	{
		ctx.SetTextureData(*this, src, pixStride, srcDim);
		this->pixelStride = pixStride;
	}
	else
	{
		pRes.Reset();
		*this = std::move(Texture2D(
			GetDevice(),
			srcDim,
			GetFormat(),
			GetUsage(),
			GetBindFlags(),
			GetAccessFlags(),
			desc.MipLevels,
			(void*)src.GetData(), (uint)pixStride
		));
	}
}

Texture2D Texture2D::FromImageWIC(Device& dev, wstring_view file, bool isReadOnly)
{
	ScratchImage buf;
	LoadImageWIC(file, buf);

	if (buf.GetImage(0, 0, 0)->format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		ScratchImage converted;
		DirectX::Convert(*buf.GetImage(0, 0, 0), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
		buf = std::move(converted);
	}

	const Image& img = *buf.GetImage(0, 0, 0);

	return Texture2D(dev,
		uivec2(img.width, img.height),
		img.pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM,
		isReadOnly
	);
}
