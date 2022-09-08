#include "D3D11/Device.hpp"
#include "D3D11/Resources/Formats.hpp"
#include "D3D11/Resources/BufferBase.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>

using namespace DirectX;
using namespace Replica::D3D11;

Texture2D::Texture2D(
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
	ResourceBase(dev)
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
		GFX_THROW_FAILED(GetDevice()->CreateTexture2D(&desc, &initData, &pRes));
	}
	else
	{
		GFX_THROW_FAILED(GetDevice()->CreateTexture2D(&desc, nullptr, &pRes));
	}

	if ((int)(bindFlags & ResourceBindFlags::ShaderResource))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = desc.Format;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.Texture2D.MipLevels = desc.MipLevels;

		GFX_THROW_FAILED(GetDevice()->CreateShaderResourceView(pRes.Get(), &vDesc, &pSRV));
	}
}

Texture2D::Texture2D(
	Device& dev,
	ivec2 dim,
	void* data,
	UINT stride,
	Formats format,
	ResourceUsages usage,
	UINT mipLevels
) : 
	Texture2D(
		dev, 
		dim, 
		format, 
		usage, 
		ResourceBindFlags::ShaderResource,
		(usage == ResourceUsages::Dynamic) ? ResourceAccessFlags::Write : ResourceAccessFlags::None,
		1u, 
		1u,
		data, 
		stride
	)
{ }

Texture2D::Texture2D() : desc({}) {}

ivec2 Texture2D::GetSize() const { return ivec2(desc.Width, desc.Height); }

Formats Texture2D::GetFormat() const { return (Formats)desc.Format; }

ResourceUsages Texture2D::GetUsage() const { return (ResourceUsages)desc.Usage; }

ResourceBindFlags Texture2D::GetBindFlags() const { return (ResourceBindFlags)desc.BindFlags; }

ResourceAccessFlags Texture2D::GetAccessFlags() const { return (ResourceAccessFlags)desc.CPUAccessFlags; }

/// <summary>
/// Returns interface to resource
/// </summary>
ID3D11Resource* Texture2D::GetResource() { return pRes.Get(); }

void Texture2D::UpdateTextureWIC(Context& ctx, wstring_view file, ScratchImage& buffer)
{
	LoadImageWIC(file, buffer);
	const Image& img = *buffer.GetImage(0, 0, 0);
	UpdateTextureData(ctx, img.pixels, 4 * sizeof(uint8_t), ivec2(img.width, img.height));
}

void Texture2D::UpdateMapUnmap(Context& ctx, void* data, size_t stride, ivec2 dim)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GFX_THROW_FAILED(ctx->Map(
		pRes.Get(),
		0u,
		D3D11_MAP_WRITE_DISCARD,
		0u,
		&msr
	));

	memcpy(msr.pData, data, stride * dim.x * dim.y);
	ctx->Unmap(pRes.Get(), 0u);
}

void Texture2D::UpdateSubresource(Context& ctx, void* data, size_t stride, ivec2 dim)
{
	D3D11_BOX dstBox;
	dstBox.left = 0;
	dstBox.right = dim.x;
	dstBox.top = 0;
	dstBox.bottom = dim.y;
	dstBox.front = 0;
	dstBox.back = 1;

	ctx->UpdateSubresource(
		pRes.Get(),
		0,
		&dstBox,
		data,
		stride * dim.x,
		stride * dim.x * dim.y
	);
}

void Texture2D::UpdateTextureData(Context& ctx, void* data, size_t stride, ivec2 dim)
{
	pRes->GetDesc(&desc);

	if (dim == GetSize())
	{ 
		GFX_ASSERT(GetUsage() != ResourceUsages::Immutable, "Cannot update Textures without write access.");

		if (GetUsage() == ResourceUsages::Dynamic)
		{
			UpdateMapUnmap(ctx, data, stride, dim);
		}
		else
		{
			UpdateSubresource(ctx, data, stride, dim);
		}
	}
	else
	{
		pRes.Reset();
		*this = std::move(Texture2D(
			GetDevice(), 
			dim, 
			GetFormat(), 
			GetUsage(), 
			GetBindFlags(), 
			GetAccessFlags(), 
			desc.MipLevels,
			desc.ArraySize,
			data,
			stride
		));
	}
}

void Texture2D::LoadImageWIC(wstring_view file, ScratchImage& buffer)
{
	GFX_THROW_FAILED(LoadFromWICFile(
		file.data(),
		WIC_FLAGS::WIC_FLAGS_FORCE_RGB,
		nullptr,
		buffer
	));
}

Texture2D Texture2D::FromImageWIC(Device& dev, wstring_view file, ResourceUsages usage)
{
	ScratchImage buf;
	LoadImageWIC(file, buf);
	const Image& img = *buf.GetImage(0, 0, 0);

	return Texture2D(dev, 
		ivec2(img.width, img.height),
		img.pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM,
		usage
	);
}

ID3D11Resource** const Texture2D::GetResAddress() { return reinterpret_cast<ID3D11Resource**>(pRes.GetAddressOf()); }

ID3D11ShaderResourceView* Texture2D::GetSRV() { return pSRV.Get(); }

ID3D11ShaderResourceView** const Texture2D::GetSRVAddress() { return pSRV.GetAddressOf(); }
