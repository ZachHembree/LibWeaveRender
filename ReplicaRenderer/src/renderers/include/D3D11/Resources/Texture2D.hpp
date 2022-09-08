#pragma once
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/Formats.hpp"

namespace DirectX
{
	class ScratchImage;
}

namespace Replica::D3D11
{
	class Texture2D : public ResourceBase
	{
	public:

		template<typename T>
		Texture2D(
			Device& dev,
			ivec2 dim,
			IDynamicCollection<T> data,
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

		Texture2D();

		ivec2 GetSize() const;

		Formats GetFormat() const;

		ResourceUsages GetUsage() const;

		ResourceTypes GetBindFlags() const;

		ResourceAccessFlags GetAccessFlags() const;

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* GetResource();

		/// <summary>
		/// Returns pointer to interface pointer field
		/// </summary>
		ID3D11Resource** const GetResAddress() override;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetSRV();

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView** const GetSRVAddress();

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		template<typename T>
		void UpdateTextureData(Context& ctx, IDynamicCollection<T> data, ivec2 dim)
		{
			UpdateTextureData(ctx, data.GetPtr(), sizeof(T), dim);
		}

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void UpdateTextureData(Context& ctx, void* data, size_t stride, ivec2 dim);

		/// <summary>
		/// Updates texture with contents of a scratch image, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void UpdateTextureWIC(Context& ctx, wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Loads WIC-compatible image into a buffer
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static void LoadImageWIC(wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static Texture2D FromImageWIC(Device& dev, wstring_view file);

	protected:
		ComPtr<ID3D11Texture2D> pRes;
		ComPtr<ID3D11ShaderResourceView> pRTV;
		D3D11_TEXTURE2D_DESC desc;

		Texture2D(
			Device& dev,
			ivec2 dim,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceTypes bindFlags = ResourceTypes::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			UINT mipLevels = 1u,
			UINT arraySize = 1u,
			void* data = nullptr,
			UINT stride = 0
		);

	};
}