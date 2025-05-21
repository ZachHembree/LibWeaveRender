#include "pch.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/StagingTexture2D.hpp"
#include "D3D11/Device.hpp"

using namespace DirectX;
using namespace Weave;
using namespace Weave::D3D11;

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

void StagingTexture2D::Init() {}

/// <summary>
/// Updates texture with contents of a scratch image, assuming compatible formats.
/// Allocates new Texture2D if the dimensions aren't the same.
/// </summary>
void StagingTexture2D::SetTextureWIC(CtxBase& ctx, wstring_view file, DirectX::ScratchImage& buffer)
{
	LoadImageWIC(file, buffer);
	const Image& img = *buffer.GetImage(0, 0, 0);
	const uint pixStride = 4 * sizeof(byte);
	const ivec2 dim(img.width, img.height);
	const uint totalBytes = pixStride * dim.x * dim.y;
	Span srcBytes(img.pixels, totalBytes);

	SetTextureData(ctx, srcBytes, pixStride, dim);
}

/// <summary>
/// Returns a temporary handle to a mapped buffer backing the texture
/// </summary>
MappedBufferHandle StagingTexture2D::GetBufferHandle(CtxImm& ctx)
{
	return ctx.GetMappedBufferHandle(*this);
}

/// <summary>
/// Returns and unmaps a previously acquired buffer handle accessed using GetBufferHandle()
/// </summary>
void StagingTexture2D::ReturnBufferHandle(CtxBase& ctx, MappedBufferHandle&& handle)
{
	D3D_CHECK_MSG(GetIsValid(),
		"Cannot return a null Texture Buffer Handle");

	D3D_CHECK_MSG(&handle.GetParent() == this,
		"Cannot return a Texture Buffer Handle to an object"
		"that is not its parent");

	ctx.ReturnMappedBufferHandle(std::move(handle));
}

void StagingTexture2D::WriteToFileWIC(CtxImm& ctx, string_view path)
{
	wstring widePath = GetWideString_UTF8_TO_UTF16LE(path);
	WriteToFileWIC(ctx, widePath);
}

void StagingTexture2D::WriteToFileWIC(CtxImm& ctx, wstring_view path)
{
	const ivec2 size = GetSize();
	MappedBufferHandle buffer = GetBufferHandle(ctx);
	Image imageData = 
	{
		(uint)size.x, (uint)size.y,
		(DXGI_FORMAT)desc.format,
		buffer.GetRowPitch(),
		buffer.GetByteSize(),
		buffer.GetData()
	};

	D3D_CHECK_HR(SaveToWICFile(
		imageData, 
		WIC_FLAGS_FORCE_SRGB,
		GetWICCodec(WIC_CODEC_PNG), 
		path.data()
	));

	ReturnBufferHandle(ctx, std::move(buffer));
}
