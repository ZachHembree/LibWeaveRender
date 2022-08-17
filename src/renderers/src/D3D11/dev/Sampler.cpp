#include "D3D11/dev/Sampler.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;

Sampler::Sampler(Device* pDev, TexFilterMode filter, TexClampMode u, TexClampMode v, TexClampMode w) :
	DeviceChild(pDev)
{
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = (D3D11_FILTER)filter;
	desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)u;
	desc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)v;
	desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)w;

	GFX_THROW_FAILED(pDev->Get()->CreateSamplerState(&desc, &pSamp));
}

Sampler::Sampler(Device* pDev,
	TexFilterMode filter,
	TexClampMode mode
) :
	Sampler(pDev, filter, mode, mode, mode)
{ }

ID3D11SamplerState* Sampler::Get() { return pSamp.Get(); }

void Sampler::Bind(Context& ctx, UINT slot)
{
	ctx.Get()->PSSetSamplers(slot, 1u, pSamp.GetAddressOf());
}