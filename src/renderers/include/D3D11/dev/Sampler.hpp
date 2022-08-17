#pragma once
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Determines filter mode for minification/magnification on all three potential axes of a texture (U/V/W)
	/// </summary>
	enum class TexFilterMode
	{
        POINT = D3D11_FILTER_MIN_MAG_MIP_POINT,
        MIN_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
        MIN_POINT_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
        MIN_POINT_MAG_MIP_LINEAR = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
        MIN_LINEAR_MAG_MIP_POINT = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
        MIN_LINEAR_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        MIN_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
        LINEAR = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        ANISOTROPIC = D3D11_FILTER_ANISOTROPIC,
        COMPARISON_POINT = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
        COMPARISON_MIN_MAG_POINT_MIP_LINEAR = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
        COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
        COMPARISON_MIN_POINT_MAG_MIP_LINEAR = D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
        COMPARISON_MIN_LINEAR_MAG_MIP_POINT = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
        COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        COMPARISON_MIN_MAG_LINEAR_MIP_POINT = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
        COMPARISON_LINEAR = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
        COMPARISON_ANISOTROPIC = D3D11_FILTER_COMPARISON_ANISOTROPIC,
        MINIMUM_POINT = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
        MINIMUM_MIN_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
        MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
        MINIMUM_MIN_POINT_MAG_MIP_LINEAR = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
        MINIMUM_MIN_LINEAR_MAG_MIP_POINT = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
        MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        MINIMUM_MIN_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
        MINIMUM_LINEAR = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
        MINIMUM_ANISOTROPIC = D3D11_FILTER_MINIMUM_ANISOTROPIC,
        MAXIMUM_POINT = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
        MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
        MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
        MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
        MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
        MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
        MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
        MAXIMUM_LINEAR = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
        MAXIMUM_ANISOTROPIC = D3D11_FILTER_MAXIMUM_ANISOTROPIC
	};

    /// <summary>
    /// Determines how texture sampling is/isn't bounded
    /// </summary>
    enum class TexClampMode
    {
        WRAP = D3D11_TEXTURE_ADDRESS_WRAP,
        MIRROR = D3D11_TEXTURE_ADDRESS_MIRROR,
        CLAMP = D3D11_TEXTURE_ADDRESS_CLAMP,
        BORDER = D3D11_TEXTURE_ADDRESS_BORDER,
        MIRROR_ONCE = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE
    };

    /// <summary>
    /// Comparison functions used to determine when new (source) data is to be
    /// stored in a given destination. Typically used in depth-stencil buffers.
    /// </summary>
    enum class TexCmpFunc
    {
        NEVER = D3D11_COMPARISON_NEVER,
        LESS = D3D11_COMPARISON_LESS,
        EQUAL = D3D11_COMPARISON_EQUAL,
        LESS_EQUAL = D3D11_COMPARISON_LESS_EQUAL,
        GREATER = D3D11_COMPARISON_GREATER,
        NOT_EQUAL = D3D11_COMPARISON_NOT_EQUAL,
        GREATER_EQUAL = D3D11_COMPARISON_GREATER_EQUAL,
        ALWAYS = D3D11_COMPARISON_ALWAYS
    };

	class Sampler : public DeviceChild
	{
	public:
		Sampler(Device* pDev, 
            TexFilterMode filter, 
            TexClampMode u, 
            TexClampMode v, 
            TexClampMode w
        );

        Sampler(Device* pDev, 
            TexFilterMode filter = TexFilterMode::LINEAR, 
            TexClampMode mode = TexClampMode::WRAP
        );

        /// <summary>
        /// Returns pointer to COM interface
        /// </summary>
        ID3D11SamplerState* Get();

        /// <summary>
        /// Binds the sampler to the pixel shader stage
        /// </summary>
        void Bind(Context& ctx, UINT slot = 0u);

	private:
		ComPtr<ID3D11SamplerState> pSamp;
	};
}