#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

DEF_DEST_MOVE(Sampler);

Sampler::Sampler()
{ }

Sampler::Sampler(Device& dev, TexFilterModes filter, TexClampModes u, TexClampModes v, TexClampModes w) :
	DeviceChild(dev)
{
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = (D3D11_FILTER)filter;
	desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)u;
	desc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)v;
	desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)w;
	
	D3D_CHECK_HR(dev->CreateSamplerState(&desc, &pSamp));
}

Sampler::Sampler(Device& dev,
	TexFilterModes filter,
	TexClampModes mode
) :
	Sampler(dev, filter, mode, mode, mode)
{ }

ID3D11SamplerState* Sampler::Get() const { return pSamp.Get(); }

ID3D11SamplerState* const* Sampler::GetAddressOf() const { return pSamp.GetAddressOf(); }

Sampler::operator ID3D11SamplerState* () const { return pSamp.Get(); }
