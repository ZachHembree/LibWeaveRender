#include "D3D11/Resources/RenderTarget.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Device.hpp"

using namespace Replica::D3D11;

RTHandle::RTHandle()
{ }

RTHandle::RTHandle(
	Device& dev,
	ID3D11RenderTargetView** const ppRTV
) :
	DeviceChild(dev),
	ppRTV(ppRTV)
{ }

ID3D11RenderTargetView* RTHandle::GetRTV() { return *ppRTV; }

ID3D11RenderTargetView** const RTHandle::GetAddressRTV() { return ppRTV; }

void RTHandle::Clear(
	Context& ctx,
	vec4 color
)
{
	if (ppRTV != nullptr)
	{ 
		ctx.Get()->ClearRenderTargetView(*ppRTV, reinterpret_cast<float*>(&color));
	}
}
