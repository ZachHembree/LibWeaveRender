#pragma once
#include "ResizeableTexture2D.hpp"
#include "../Resources/RWTexture2D.hpp"

namespace Weave::D3D11
{
	class RWTexture2D : public virtual IRWTexture2D, public ResizeableTexture2D
	{
	public:
		RWTexture2D();

		RWTexture2D(
			Device& dev,
			uivec2 dim,
			void* data,
			UINT stride,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		);

		RWTexture2D(
			Device& dev,
			Formats format,
			uivec2 dim = uivec2(0),
			UINT mipLevels = 1u
		);

		template<typename T>
		RWTexture2D(
			Device& dev,
			uivec2 dim,
			IDynamicArray<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		)
			: RWTexture2D(dev, dim, data.GetData(), sizeof(T), format, mipLevels)
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
		/// Clears the texture to the given color
		/// </summary>
		void Clear(CtxBase& ctx, vec4 color = vec4(0)) override;

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
		void SetTextureData(CtxBase& ctx, IDynamicArray<T>& src, uivec2 dim)
		{
			Span<byte> srcBytes(reinterpret_cast<byte*>(src.GetData()), GetArrSize(src));
			SetTextureData(ctx, srcBytes, sizeof(T), dim);
		}

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureData(CtxBase& ctx, const IDynamicArray<byte>& src, uint pixStride, uivec2 srcDim) override;

	private:
		ComPtr<ID3D11UnorderedAccessView> pUAV;
		ComPtr<ID3D11RenderTargetView> pRTV;
		mutable vec2 renderOffset;
		mutable vec2 renderScale;

		RWTexture2D(
			Device& dev,
			uivec2 dim,
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