#include "D3D11/dev/RenderTarget.hpp"
#include "D3D11/dev/Context.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;

RenderTarget::RenderTarget(Device* pDev, ID3D11Resource* pRes) :
	DeviceChild(pDev),
	pRes(pRes)
{
	GFX_THROW_FAILED(pDev->Get()->CreateRenderTargetView(pRes, nullptr, &pView));
}

RenderTarget::RenderTarget(RenderTarget&& other) noexcept :
	DeviceChild(other.pDev),
	pRes(std::move(pRes)),
	pView(std::move(pView))
{
	other.pDev = nullptr;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& other) noexcept
{
	this->pRes = std::move(other.pRes);
	this->pView = std::move(other.pView);
	this->pDev = other.pDev;
	other.pDev = nullptr;

	return *this;
}

ID3D11Resource* RenderTarget::Get() { return pRes.Get(); }

ID3D11RenderTargetView* RenderTarget::GetView() { return pView.Get(); }

void RenderTarget::Bind(Context& ctx)
{
	ctx.Get()->OMSetRenderTargets(1, pView.GetAddressOf(), nullptr);
}

void RenderTarget::Clear(Context& ctx, vec4 color)
{
	ctx.Get()->ClearRenderTargetView(GetView(), reinterpret_cast<float*>(&color));
}
