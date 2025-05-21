#pragma once
#include "ResourceBase.hpp"

namespace Weave::D3D11
{
	class SwapChain;

	/// <summary>
	/// Handle to internally managed Render Texture handle
	/// </summary>
	class RTHandle : public DeviceChild, public IRenderTarget
	{
	public:
		MAKE_MOVE_ONLY(RTHandle)

		RTHandle();
		
		RTHandle(
			Device& dev, 
			SwapChain* pSwapChain,
			ID3D11Resource** ppRes,
			ID3D11RenderTargetView** ppRTV,
			uivec2 offset = uivec2(0),
			vec2 scale = vec2(1)
		);

		/// <summary>
		/// Returns the dimensions of the underlying buffer
		/// </summary>
		uivec3 GetDimensions() const override;

		/// <summary>
		/// Specifies buffer usage pattern: dynamic, staging, immutable or default
		/// </summary>
		ResourceUsages GetUsage() const override;

		/// <summary>
		/// Specifies what view types for the buffer are valid
		/// </summary>
		ResourceBindFlags GetBindFlags() const override;

		/// <summary>
		/// Specifies CPU access type for mappable resource, if applicable
		/// </summary>
		ResourceAccessFlags GetAccessFlags() const override;

		/// <summary>
		/// Returns a pointer to the resource interface
		/// </summary>
		ID3D11Resource* GetResource() const override;

		/// <summary>
		/// Returns a pointer to the resource field address
		/// </summary>
		ID3D11Resource* const* GetResAddress() const;

		/// <summary>
		/// Returns the dimensions of the 2D texture
		/// </summary>
		uivec2 GetSize() const override;

		/// <summary>
		/// Returns combined texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		vec4 GetTexelSize() const override;

		/// <summary>
		/// Returns color format of the underlying buffer
		/// </summary>
		Formats GetFormat() const override;

		/// <summary>
		/// Sets the offset for this target in pixels
		/// </summary>
		void SetRenderOffset(ivec2 offset);

		/// <summary>
		/// Returns the offset set for this target in pixels
		/// </summary>
		ivec2 GetRenderOffset() const override;

		/// <summary>
		/// Sets the size of the render area for the render texture.
		/// Cannot exceed the size of the underlying buffer.
		/// </summary>
		void SetRenderSize(uivec2 renderSize);

		/// <summary>
		/// Returns the size of the render area in pixels
		/// </summary>
		uivec2 GetRenderSize() const override;

		/// <summary>
		/// Returns combined scaled (DRS) texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		vec4 GetRenderTexelSize() const;

		/// <summary>
		/// Sets the renderSize to size ratio on (0, 1].
		/// </summary>
		void SetRenderScale(vec2 scale);

		/// <summary>
		/// Returns the renderSize to size ratio on (0, 1].
		/// </summary>
		vec2 GetRenderScale() const override;

		/// <summary>
		/// Read-only pointer to pointer for Render Target view
		/// </summary>
		ID3D11RenderTargetView* GetRTV() override;

		ID3D11RenderTargetView* const* GetAddressRTV() override;

		/// <summary>
		/// Clears the render target to the given color
		/// </summary>
		void Clear(
			CtxBase& ctx,
			vec4 color = vec4(0)
		) override;

	private:
		ID3D11Resource** ppRes;
		ID3D11RenderTargetView** ppRTV;
		SwapChain* pSwapChain;
		mutable vec2 renderOffset;
		mutable vec2 renderScale;
	};
}