#pragma once
#include "Texture2D.hpp"

namespace Replica::D3D11
{
	class RWTexture2D : public virtual IRWTexture2D, public Texture2D
	{
	public:
		RWTexture2D();

		RWTexture2D(
			Device& dev,
			ivec2 dim,
			void* data,
			UINT stride,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		);

		RWTexture2D(
			Device& dev,
			Formats format,
			ivec2 dim = ivec2(0),
			UINT mipLevels = 1u
		);

		template<typename T>
		RWTexture2D(
			Device& dev,
			ivec2 dim,
			IDynamicArray<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		)
			: RWTexture2D(dev, dim, data.GetPtr(), sizeof(T), format, mipLevels)
		{ }

		/// <summary>
		/// Read-only pointer to pointer for Render Target view
		/// </summary>
		ID3D11RenderTargetView* GetRTV() override;

		/// <summary>
		/// Binds the render target to the output merger
		/// </summary>
		ID3D11RenderTargetView** const GetAddressRTV() override;

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
		/// Clears the texture to the given color
		/// </summary>
		void Clear(
			Context& ctx,
			vec4 color = vec4(0)
		) override;

		/// <summary>
		/// Returns pointer to UAV interface
		/// </summary>
		ID3D11UnorderedAccessView* GetUAV() override;

		/// <summary>
		/// Returns pointer to UAV pointer field
		/// </summary>
		ID3D11UnorderedAccessView** const GetAddressUAV() override;

		/// <summary>
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static RWTexture2D FromImageWIC(
			Device& dev,
			wstring_view file
		);

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		template<typename T>
		void SetTextureData(Context& ctx, IDynamicArray<T>& data, ivec2 dim)
		{
			SetTextureData(ctx, data.GetPtr(), sizeof(T), dim);
		}

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim) override;

	private:
		ComPtr<ID3D11UnorderedAccessView> pUAV;
		ComPtr< ID3D11RenderTargetView> pRTV;
		mutable vec2 renderOffset;
		mutable vec2 renderScale;

		RWTexture2D(
			Device& dev,
			ivec2 dim,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceBindFlags bindFlags = ResourceBindFlags::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			UINT mipLevels = 1u,
			void* data = nullptr,
			UINT stride = 0
		);
	};
}