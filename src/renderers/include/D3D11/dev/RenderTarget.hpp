#pragma once
#include "D3D11/dev/ResourceBase.hpp"

namespace Replica::D3D11
{
	class RenderTarget : public ResourceBase
	{
	public:
		RenderTarget(Device* pDev, ID3D11Resource* pRes);

		ID3D11Resource* GetResource() { return pRes.Get(); }

		ID3D11Resource** GetResAddress() { return pRes.GetAddressOf(); }

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11RenderTargetView* GetView();

		/// <summary>
		/// Binds the render target to the output merger
		/// </summary>
		void Bind(Context& ctx);

		/// <summary>
		/// Clears the render target to the given color
		/// </summary>
		void Clear(Context& ctx, vec4 color = vec4(0));

	private:
		ComPtr<ID3D11Resource> pRes;
		ComPtr<ID3D11RenderTargetView> pView;
	};
}