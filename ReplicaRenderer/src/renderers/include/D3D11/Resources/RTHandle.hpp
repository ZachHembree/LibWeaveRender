#pragma once
#include "D3D11/Resources/ResourceBase.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Handle to internally managed Render Texture handle
	/// </summary>
	class RTHandle : public DeviceChild, public IRenderTarget
	{
	public:
		RTHandle();
		
		RTHandle(
			Device& dev, 
			ID3D11RenderTargetView** ppRTV
		);

		/// <summary>
		/// Read-only pointer to pointer for Render Target view
		/// </summary>
		ID3D11RenderTargetView* GetRTV() override;

		/// <summary>
		/// Binds the render target to the output merger
		/// </summary>
		ID3D11RenderTargetView** const GetAddressRTV() override;

		/// <summary>
		/// Clears the render target to the given color
		/// </summary>
		void Clear(
			Context& ctx,
			vec4 color = vec4(0)
		) override;

	private:
		ID3D11RenderTargetView** ppRTV;
	};
}