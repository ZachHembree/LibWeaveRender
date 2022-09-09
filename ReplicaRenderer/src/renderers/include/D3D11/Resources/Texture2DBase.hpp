#pragma once
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/Formats.hpp"

namespace DirectX
{
	class ScratchImage;
}

namespace Replica::D3D11
{
	class Texture2DBase : public ResourceBase
	{
	public:

		ivec2 GetSize() const;

		Formats GetFormat() const;

		ResourceUsages GetUsage() const;

		ResourceBindFlags GetBindFlags() const;

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
		/// Loads WIC-compatible image into a buffer
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static void LoadImageWIC(wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Returns true if the object is valid and has been initialized
		/// </summary>
		bool GetIsValid() const override;

	protected:
		ComPtr<ID3D11Texture2D> pRes;
		D3D11_TEXTURE2D_DESC desc;

		Texture2DBase();

		Texture2DBase(
			Device& dev,
			ivec2 dim,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceBindFlags bindFlags = ResourceBindFlags::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			UINT mipLevels = 1u,
			UINT arraySize = 1u,
			void* data = nullptr,
			UINT stride = 0
		);

		void UpdateMapUnmap(Context& ctx, void* data, size_t stride, ivec2 dim);

		void UpdateSubresource(Context& ctx, void* data, size_t stride, ivec2 dim);
	};
}