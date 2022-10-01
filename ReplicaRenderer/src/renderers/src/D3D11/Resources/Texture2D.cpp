#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "ReplicaInternalD3D11.hpp"

using namespace DirectX;
using namespace Replica::D3D11;

Texture2D::Texture2D() 
{ }

Texture2D::Texture2D(
	Device& dev,
	ivec2 dim,
	void* data,
	UINT stride,
	Formats format,
	UINT mipLevels,
	bool isReadonly
) :
	Texture2DBase(
		dev,
		dim,
		format,
		isReadonly ? ResourceUsages::Immutable : ResourceUsages::Default,
		ResourceBindFlags::ShaderResource,
		ResourceAccessFlags::None,
		1u, 1u,
		data, stride
	)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
	vDesc.Format = desc.Format;
	vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	vDesc.Texture2D.MostDetailedMip = 0;
	vDesc.Texture2D.MipLevels = desc.MipLevels;

	GFX_THROW_FAILED(dev->CreateShaderResourceView(pRes.Get(), &vDesc, &pSRV));
}

Texture2D::Texture2D(
	Device& dev,
	Formats format,
	ivec2 dim,
	UINT mipLevels
) :
	Texture2DBase(
		dev,
		dim,
		format,
		ResourceUsages::Default,
		ResourceBindFlags::ShaderResource,
		ResourceAccessFlags::None,
		1u, 1u,
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

void Texture2D::SetTextureWIC(Context& ctx, wstring_view file, ScratchImage& buffer)
{
	LoadImageWIC(file, buffer);
	const Image& img = *buffer.GetImage(0, 0, 0);
	SetTextureData(ctx, img.pixels, 4 * sizeof(uint8_t), ivec2(img.width, img.height));
}

void Texture2D::SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim)
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
		*this = std::move(Texture2D(
			GetDevice(),
			dim,
			data,
			(UINT)stride,
			GetFormat(),
			GetIsReadOnly(),
			desc.MipLevels
		));
	}

	pRes->GetDesc(&desc);
}

Texture2D Texture2D::FromImageWIC(Device& dev, wstring_view file, bool isReadOnly)
{
	ScratchImage buf;
	LoadImageWIC(file, buf);
	const Image& img = *buf.GetImage(0, 0, 0);

	return Texture2D(dev,
		ivec2(img.width, img.height),
		img.pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM,
		isReadOnly
	);
}
