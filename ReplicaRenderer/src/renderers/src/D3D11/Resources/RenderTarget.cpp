#include "D3D11/Resources/RenderTarget.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Device.hpp"

using namespace Replica::D3D11;

RenderTarget::RenderTarget() : pDSView(nullptr)
{ }

RenderTarget::RenderTarget(Device& dev, ID3D11Resource* pRes, ID3D11DepthStencilView* pDSV) :
	ResourceBase(dev),
	pRes(pRes),
	pDSView(pDSV)
{
	GFX_THROW_FAILED(pDev->Get()->CreateRenderTargetView(pRes, nullptr, &pView));
}

RenderTarget::~RenderTarget()
{
	pRes->Release();
}

ID3D11RenderTargetView* RenderTarget::GetView() { return pView.Get(); }

void RenderTarget::Bind(Context& ctx)
{
	ctx.Get()->OMSetRenderTargets(1, pView.GetAddressOf(), pDSView);
}

void RenderTarget::Clear(Context& ctx, vec4 color)
{
	ctx.Get()->ClearRenderTargetView(GetView(), reinterpret_cast<float*>(&color));
	ctx.Get()->ClearDepthStencilView(pDSView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
