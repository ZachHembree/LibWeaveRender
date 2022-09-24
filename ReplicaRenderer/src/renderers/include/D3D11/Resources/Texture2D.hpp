#pragma once
#include "Texture2DBase.hpp"

namespace DirectX
{
	class ScratchImage;
}

namespace Replica::D3D11
{
	class Texture2D : public virtual ITexture2D, public Texture2DBase
	{
	public:

		template<typename T>
		Texture2D(
			Device& dev,
			ivec2 dim,
			IDynamicArray<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			UINT mipLevels = 1u
		)
			: Texture2D(dev, dim, data.GetPtr(), sizeof(T), format, usage, mipLevels)
		{ }

		Texture2D(
			Device& dev,
			ivec2 dim,
			void* data,
			UINT stride,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			UINT mipLevels = 1u
		);

		Texture2D(
			Device& dev,
			Formats format = Formats::R8G8B8A8_UNORM,
			ivec2 dim = ivec2(0),
			ResourceUsages usage = ResourceUsages::Default,
			UINT mipLevels = 1u
		);

		Texture2D();

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetSRV() override;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView** const GetSRVAddress() override;

		/// <summary>
		/// Updates texture with contents of a scratch image, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureWIC(Context& ctx, wstring_view file, DirectX::ScratchImage& buffer);

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
		virtual void SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim);

		/// <summary>
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static Texture2D FromImageWIC(
			Device& dev,
			wstring_view file,
			ResourceUsages usage = ResourceUsages::Immutable
		);
		
	protected:
		using Texture2DBase::Texture2DBase;
		ComPtr<ID3D11ShaderResourceView> pSRV;

	};
}