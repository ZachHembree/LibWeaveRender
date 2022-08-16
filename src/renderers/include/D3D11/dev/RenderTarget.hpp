#pragma once
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	class RenderTarget : public DeviceChild
	{
	public:
		RenderTarget(Device* pDev, ID3D11Resource* pRes);

		RenderTarget(RenderTarget&& other) noexcept;

		RenderTarget& operator=(RenderTarget&& other) noexcept;

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* Get();

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