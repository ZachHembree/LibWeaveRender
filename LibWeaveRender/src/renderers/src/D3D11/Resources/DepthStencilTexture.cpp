#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/DepthStencilTexture.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(DepthStencilTexture);

DepthStencilTexture::DepthStencilTexture()
{ }

DepthStencilTexture::DepthStencilTexture(
	Device& dev,
	ivec2 dim,
	vec2 range,
	Formats format,
	ResourceUsages usage,
	TexCmpFuncs depthCmp
) :
	Texture2DBase(dev, dim, format, usage, ResourceBindFlags::DepthStencil ),
	range(range),
	depthCmp(depthCmp)
{ Init(); }

void DepthStencilTexture::SetRange(vec2 range)
{
	this->range = range;
}

vec2 DepthStencilTexture::GetRange() const
{
	return range;
}

ID3D11DepthStencilState* DepthStencilTexture::GetState() { return pState.Get(); }

ID3D11DepthStencilView* DepthStencilTexture::GetDSV() { return pDSV.Get(); }

ID3D11DepthStencilView* const* DepthStencilTexture::GetDSVAddress() { return pDSV.GetAddressOf(); }

void DepthStencilTexture::Clear(CtxBase& ctx, DSClearFlags clearFlags, float depthClear, byte stencilClear )
{
	ctx.ClearDepthStencil(*this, clearFlags, depthClear, stencilClear);
}

void DepthStencilTexture::Init()
{
	// State
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = (D3D11_COMPARISON_FUNC)depthCmp;

	D3D_CHECK_HR(GetDevice()->CreateDepthStencilState(&dsDesc, &pState));

	// View
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = (DXGI_FORMAT)desc.format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;

	D3D_CHECK_HR(GetDevice()->CreateDepthStencilView(pRes.Get(), &descDSV, &pDSV));
}
