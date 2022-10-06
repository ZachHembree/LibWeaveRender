#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "ReplicaInternalD3D11.hpp"

using namespace DirectX;
using namespace Replica;
using namespace Replica::D3D11;

/// <summary>
/// Constructs an uninitialized texture object
/// </summary>
StagingTexture2D::StagingTexture2D() { }

/// <summary>
/// Constructs and allocates a staging texture with the given format and size.
/// CPU access flags cannot be none.
/// </summary>
StagingTexture2D::StagingTexture2D(Device& dev, Formats format, 
	ivec2 dim, uint mipLevels, ResourceAccessFlags accessFlags
) :
	Texture2DBase(
		dev,
		dim,
		format,
		ResourceUsages::Staging,
		ResourceBindFlags::None,
		accessFlags == ResourceAccessFlags::None ? ResourceAccessFlags::Write : accessFlags,
		1u, 1u,
		nullptr, 0u
	)
{ }

StagingTexture2D::StagingTexture2D(Device& dev, ivec2 dim, void* data, 
	uint stride, Formats format, uint mipLevels, ResourceAccessFlags accessFlags
) :
	Texture2DBase(
		dev,
		dim,
		format,
		ResourceUsages::Staging,
		ResourceBindFlags::None,
		accessFlags == ResourceAccessFlags::None ? ResourceAccessFlags::Write : accessFlags,
		1u, 1u,
		data, stride
	)
{ }

/// <summary>
/// Updates texture with contents of a scratch image, assuming compatible formats.
/// Allocates new Texture2D if the dimensions aren't the same.
/// </summary>
void StagingTexture2D::SetTextureWIC(Context& ctx, wstring_view file, DirectX::ScratchImage& buffer)
{
	LoadImageWIC(file, buffer);
	const Image& img = *buffer.GetImage(0, 0, 0);
	SetTextureData(ctx, img.pixels, 4 * sizeof(uint8_t), ivec2(img.width, img.height));
}

/// <summary>
/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
/// Allocates new Texture2D if the dimensions aren't the same.
/// </summary>
void StagingTexture2D::SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim)
{
	const ivec2 dstSize = GetSize();

	if (dim.x <= dstSize.x && dim.y <= dstSize.y)
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
		*this = std::move(StagingTexture2D(
			GetDevice(),
			dim,
			data,
			(UINT)stride,
			GetFormat(),
			desc.MipLevels,
			GetAccessFlags()
		));
	}

	pRes->GetDesc(&desc);
}
