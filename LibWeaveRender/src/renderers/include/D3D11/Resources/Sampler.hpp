#pragma once
#include "DeviceChild.hpp"
#include "../D3D11Utils.hpp"

namespace Weave::D3D11
{
	class Sampler : public DeviceChild
	{
	public:
        DECL_DEST_MOVE(Sampler);

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
        ID3D11SamplerState* Get() const;

        /// <summary>
        /// Returns pointer to COM interface pointer
        /// </summary>
        ID3D11SamplerState* const* GetAddressOf() const;

	private:
        UniqueComPtr<ID3D11SamplerState> pSamp;
	};
}