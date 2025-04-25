#pragma once
#include "DeviceChild.hpp"
#include "ResourceEnums.hpp"

struct ID3D11SamplerState;

namespace Weave::D3D11
{
	class Sampler : public DeviceChild
	{
	public:
        Sampler();

		Sampler(Device& dev, 
            TexFilterMode filter, 
            TexClampMode u, 
            TexClampMode v, 
            TexClampMode w
        );

        Sampler(Device& dev,
            TexFilterMode filter = TexFilterMode::LINEAR, 
            TexClampMode mode = TexClampMode::WRAP
        );

        /// <summary>
        /// Returns pointer to COM interface
        /// </summary>
        ID3D11SamplerState* Get();

        /// <summary>
        /// Returns pointer to COM interface pointer
        /// </summary>
        ID3D11SamplerState** GetAddressOf();

	private:
		ComPtr<ID3D11SamplerState> pSamp;
	};
}