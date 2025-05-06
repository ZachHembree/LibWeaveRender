#pragma once
#include "DeviceChild.hpp"
#include "../CommonEnums.hpp"
#include "WeaveUtils/Win32.hpp"

namespace Weave::D3D11
{
	class Sampler : public DeviceChild
	{
	public:
        Sampler();

		Sampler(Device& dev, 
            TexFilterModes filter, 
            TexClampModes u, 
            TexClampModes v, 
            TexClampModes w
        );

        Sampler(Device& dev,
            TexFilterModes filter = TexFilterModes::LINEAR, 
            TexClampModes mode = TexClampModes::WRAP
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