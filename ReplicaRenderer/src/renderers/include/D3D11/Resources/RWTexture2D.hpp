#pragma once
#include "D3D11/Resources/Texture2D.hpp"

namespace Replica::D3D11
{
	class RWTexture2D : public Texture2D, public IUnorderedAccess
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
			UINT mipLevels = 1u
		);

		template<typename T>
		RWTexture2D(
			Device& dev,
			ivec2 dim,
			IDynamicCollection<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		)
			: RWTexture2D(dev, dim, data.GetPtr(), sizeof(T), format, mipLevels)
		{ }

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim) override;

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

	private:
		ComPtr<ID3D11UnorderedAccessView> pUAV;

	};
}