#include "D3D11/Device.hpp"
#include "D3D11/Resources/DepthStencilTexture.hpp"

using namespace Replica::D3D11;

DepthStencilTexture::DepthStencilTexture()
{ }

DepthStencilTexture::DepthStencilTexture(
	Device& dev,
	ivec2 dim,
	Formats format,
	ResourceUsages usage,
	TexCmpFunc depthCmp
) :
	Texture2D(dev, dim, format, usage, ResourceBindFlags::DepthStencil )
{
	// State
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = (D3D11_COMPARISON_FUNC)depthCmp;

	GFX_THROW_FAILED(dev->CreateDepthStencilState(&dsDesc, &pState));

	// View
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = (DXGI_FORMAT)format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	GFX_THROW_FAILED(dev->CreateDepthStencilView(pRes.Get(), &descDSV, &pDSV));
}

ID3D11DepthStencilState* DepthStencilTexture::GetState() { return pState.Get(); }

ID3D11DepthStencilView* DepthStencilTexture::GetDSV() { return pDSV.Get(); }

ID3D11DepthStencilView** const DepthStencilTexture::GetDSVAddress() { return pDSV.GetAddressOf(); }

void DepthStencilTexture::Clear(Context& ctx, DSClearFlags clearFlags, float depthClear, UINT8 stencilClear ) const
{
	ctx->ClearDepthStencilView(pDSV.Get(), (UINT)clearFlags, depthClear, stencilClear);
}
