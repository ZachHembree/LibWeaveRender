#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/Formats.hpp"
#include "D3D11/dev/BufferBase.hpp"
#include "D3D11/dev/Texture2D.hpp"
#include <WICTextureLoader.h>
#include <DirectXTex.h>

using namespace DirectX;
using namespace Replica::D3D11;

Texture2D::Texture2D(Device* pDev,
	ivec2 dim,
	void* data,
	UINT stride,
	Formats format,
	UINT mipLevels
) :
	DeviceChild(pDev)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = dim.x;
	desc.Height = dim.y;
	desc.Format = (DXGI_FORMAT)format;

	desc.MipLevels = mipLevels;
	desc.ArraySize = 1;

	desc.Usage = (D3D11_USAGE)ResourceUsages::Default;
	desc.BindFlags = (D3D11_BIND_FLAG)ResourceTypes::ShaderResource;
	desc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)ResourceAccessFlags::None;
	desc.MiscFlags = 0u;

	// Multisampling
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = data;
	initData.SysMemPitch = dim.x * stride;

	GFX_THROW_FAILED(pDev->Get()->CreateTexture2D(&desc, &initData, &pRes));

	D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
	vDesc.Format = desc.Format;
	vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	vDesc.Texture2D.MostDetailedMip = 0;
	vDesc.Texture2D.MipLevels = desc.MipLevels;

	GFX_THROW_FAILED(pDev->Get()->CreateShaderResourceView(pRes.Get(), &vDesc, &pView));
}

Texture2D::Texture2D(Device* pDev,
	const wchar_t* file
) :
	DeviceChild(pDev)
{
	ScratchImage buf;
	GFX_THROW_FAILED(LoadFromWICFile(
		file,
		WIC_FLAGS::WIC_FLAGS_FORCE_RGB,
		nullptr,
		buf
	));

	const Image* img = buf.GetImage(0, 0, 0);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = img->width;
	desc.Height = img->height;
	desc.Format = img->format;

	desc.MipLevels = 1;
	desc.ArraySize = 1;

	desc.Usage = (D3D11_USAGE)ResourceUsages::Default;
	desc.BindFlags = (D3D11_BIND_FLAG)ResourceTypes::ShaderResource;
	desc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)ResourceAccessFlags::None;
	desc.MiscFlags = 0u;

	// Multisampling
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = img->pixels;
	initData.SysMemPitch = img->rowPitch;

	GFX_THROW_FAILED(pDev->Get()->CreateTexture2D(&desc, &initData, &pRes));

	D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
	vDesc.Format = desc.Format;
	vDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	vDesc.Texture2D.MostDetailedMip = 0;
	vDesc.Texture2D.MipLevels = desc.MipLevels;

	GFX_THROW_FAILED(pDev->Get()->CreateShaderResourceView(pRes.Get(), &vDesc, &pView));
}

Texture2D::Texture2D(Texture2D&& other) noexcept :
	DeviceChild(other.pDev),
	pRes(std::move(other.pRes)),
	pView(std::move(other.pView))
{
	other.pDev = nullptr;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
	this->pDev = other.pDev;
	pRes = std::move(other.pRes);
	pView = std::move(other.pView);
	other.pDev = nullptr;

	return *this;
}

/// <summary>
/// Returns interface to resource
/// </summary>
ID3D11Resource* Texture2D::Get() { return pRes.Get(); }

/// <summary>
/// Returns interface to resource view
/// </summary>
ID3D11ShaderResourceView* Texture2D::GetView() { return pView.Get(); }

void Texture2D::Bind(Context& ctx, UINT slot)
{
	ctx.Get()->PSSetShaderResources(slot, 1u, pView.GetAddressOf());
}
