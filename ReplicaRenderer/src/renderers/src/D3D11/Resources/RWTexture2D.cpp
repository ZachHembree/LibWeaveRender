#include <DirectXTex.h>
#include "ReplicaInternalD3D11.hpp"

using namespace DirectX;
using namespace Replica::D3D11;

RWTexture2D::RWTexture2D()
{ }

RWTexture2D::RWTexture2D(
	Device& dev,
	ivec2 dim,
	Formats format,
	ResourceUsages usage,
	ResourceBindFlags bindFlags,
	ResourceAccessFlags accessFlags,
	UINT mipLevels,
	UINT arraySize,
	void* data,
	UINT stride
) :
	Texture2D(
		dev,
		dim,
		format,
		usage,
		bindFlags,
		accessFlags,
		mipLevels,
		arraySize,
		data,
		stride
	)
{
	if (pRes.Get() != nullptr)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = desc.Format;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.Texture2D.MipLevels = desc.MipLevels;

		GFX_THROW_FAILED(dev->CreateShaderResourceView(pRes.Get(), &vDesc, &pSRV));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = (DXGI_FORMAT)format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		GFX_THROW_FAILED(dev->CreateUnorderedAccessView(pRes.Get(), &uavDesc, &pUAV));
	}
}

RWTexture2D::RWTexture2D(Device& dev,
	ivec2 dim,
	void* data,
	UINT stride,
	Formats format,
	UINT mipLevels
) :
	RWTexture2D(
		dev,
		dim,
		format,
		ResourceUsages::Default,
		ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess,
		ResourceAccessFlags::ReadWrite,
		mipLevels, 
		1u,
		data, stride
	)
{ }

RWTexture2D::RWTexture2D(
	Device& dev,
	Formats format,
	ivec2 dim,
	UINT mipLevels
) :
	RWTexture2D(
		dev,
		dim,
		format,
		ResourceUsages::Default,
		ResourceBindFlags::ShaderResource | ResourceBindFlags::UnorderedAccess,
		ResourceAccessFlags::ReadWrite,
		1u, 1u,
		nullptr, 0u
	)
{ }

/// <summary>
/// Returns pointer to UAV interface
/// </summary>
ID3D11UnorderedAccessView* RWTexture2D::GetUAV() { return pUAV.Get(); }

/// <summary>
/// Returns pointer to UAV pointer field
/// </summary>
ID3D11UnorderedAccessView** const RWTexture2D::GetAddressUAV() { return pUAV.GetAddressOf(); }

void RWTexture2D::SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim)
{
	if (dim == GetSize())
	{
		GFX_ASSERT(GetUsage() != ResourceUsages::Immutable, "Cannot update Textures without write access.");

		if (GetUsage() == ResourceUsages::Dynamic)
			UpdateMapUnmap(ctx, data, stride, dim);
		else
			UpdateSubresource(ctx, data, stride, dim);
	}
	else
	{
		pRes.Reset();
		*this = std::move(RWTexture2D(
			GetDevice(),
			dim,
			data,
			(UINT)stride,
			GetFormat(),
			desc.MipLevels
		));
	}

	pRes->GetDesc(&desc);
}

RWTexture2D RWTexture2D::FromImageWIC(Device& dev, wstring_view file)
{
	ScratchImage buf;
	LoadImageWIC(file, buf);
	const Image& img = *buf.GetImage(0, 0, 0);

	return RWTexture2D(dev,
		ivec2(img.width, img.height),
		img.pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM
	);
}
