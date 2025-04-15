#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

Sampler::Sampler()
{ }

Sampler::Sampler(Device& dev, TexFilterMode filter, TexClampMode u, TexClampMode v, TexClampMode w) :
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
	TexFilterMode filter,
	TexClampMode mode
) :
	Sampler(dev, filter, mode, mode, mode)
{ }

ID3D11SamplerState* Sampler::Get() { return pSamp.Get(); }

ID3D11SamplerState** Sampler::GetAddressOf() { return pSamp.GetAddressOf(); }