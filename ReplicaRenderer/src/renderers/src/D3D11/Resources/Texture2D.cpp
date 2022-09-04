#include "D3D11/Device.hpp"
#include "D3D11/Resources/Formats.hpp"
#include "D3D11/Resources/BufferBase.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>

using namespace DirectX;
using namespace Replica::D3D11;

Texture2D::Texture2D(Device* pDev,
	ivec2 dim,
	Formats format,
	ResourceUsages usage,
	ResourceTypes bindFlags,
	ResourceAccessFlags accessFlags,
	UINT mipLevels,
	UINT arraySize,
	void* data,
	UINT stride
) :
	ResourceBase(pDev)
{
	D3D11_TEXTURE2D_DESC desc = {};
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
		GFX_THROW_FAILED(pDev->Get()->CreateTexture2D(&desc, &initData, &pRes));
	}
	else
	{
		GFX_THROW_FAILED(pDev->Get()->CreateTexture2D(&desc, nullptr, &pRes));
	}

	if ((int)(bindFlags & ResourceTypes::ShaderResource))
	{ 
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = desc.Format;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vDesc.Texture2D.MostDetailedMip = 0;
		vDesc.Texture2D.MipLevels = desc.MipLevels;

		GFX_THROW_FAILED(pDev->Get()->CreateShaderResourceView(pRes.Get(), &vDesc, &pView));
	}

	if ((int)(bindFlags & ResourceTypes::DepthStencil))
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = desc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;

		GFX_THROW_FAILED(pDev->Get()->CreateDepthStencilView(pRes.Get(), &descDSV, &pDsView));
	}
}

Texture2D::Texture2D(
	Device* pDev,
	ivec2 dim,
	void* data,
	UINT stride,
	Formats format,
	UINT mipLevels
) : 
	Texture2D(
		pDev, 
		dim, 
		format, 
		ResourceUsages::Default, 
		ResourceTypes::ShaderResource, 
		ResourceAccessFlags::None,
		1u, 
		1u,
		data, 
		stride
	)
{ }

Texture2D::Texture2D() {}

Texture2D Texture2D::FromImageWIC(Device* pDev, const wchar_t* file)
{
	ScratchImage buf;
	GFX_THROW_FAILED(LoadFromWICFile(
		file,
		WIC_FLAGS::WIC_FLAGS_FORCE_RGB,
		nullptr,
		buf
	));

	const Image* img = buf.GetImage(0, 0, 0);

	return Texture2D(pDev, 
		ivec2(img->width, img->height),
		img->pixels,
		4 * sizeof(uint8_t),
		Formats::R8G8B8A8_UNORM,
		1
	);
}

ID3D11ShaderResourceView* Texture2D::GetSRV() { return pView.Get(); }

ID3D11ShaderResourceView** Texture2D::GetSRVAddress() { return pView.GetAddressOf(); }

ID3D11DepthStencilView* Texture2D::GetDSV() { return pDsView.Get(); }

ID3D11DepthStencilView** Texture2D::GetDSVAddress() { return pDsView.GetAddressOf(); }
