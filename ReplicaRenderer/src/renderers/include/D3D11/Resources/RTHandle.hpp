#pragma once
#include "ResourceBase.hpp"

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
			SwapChain* pSwapChain,
			ID3D11RenderTargetView** ppRTV,
			ivec2 offset = ivec2(0),
			vec2 scale = vec2(1)
		);

		/// <summary>
		/// Returns the size of the underlying buffer
		/// </summary>
		ivec2 GetSize() const override;

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
		void SetRenderSize(ivec2 renderSize);

		/// <summary>
		/// Returns the size of the render area in pixels
		/// </summary>
		ivec2 GetRenderSize() const override;

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
		SwapChain* pSwapChain;
		mutable vec2 renderOffset;
		mutable vec2 renderScale;
	};
}