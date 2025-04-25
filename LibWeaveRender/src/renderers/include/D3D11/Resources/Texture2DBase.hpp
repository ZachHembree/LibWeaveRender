#pragma once
#include "ResourceBase.hpp"
#include "ResourceEnums.hpp"

namespace DirectX
{
	class ScratchImage;
}

namespace Weave::D3D11
{
	class Texture2DBase : public virtual ITexture2DBase, public ResourceBase
	{
	public:
		/// <summary>
		/// Returns the dimensions of the underlying buffer
		/// </summary>
		uivec3 GetDimensions() const override;

		/// <summary>
		/// Returns the dimensions of the texture
		/// </summary>
		uivec2 GetSize() const override;

		/// <summary>
		/// Returns combined texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		vec4 GetTexelSize() const override;

		/// <summary>
		/// Returns the number of bytes per pixel for input/output
		/// </summary>
		uint GetPixelPitch() const;

		/// <summary>
		/// Returns color format of the texture
		/// </summary>
		Formats GetFormat() const override;

		/// <summary>
		/// Returns resource usage type
		/// </summary>
		ResourceUsages GetUsage() const override;

		/// <summary>
		/// Returns the view bind flags for the texture
		/// </summary>
		ResourceBindFlags GetBindFlags() const;

		/// <summary>
		/// Returns cpu access flags for the texture
		/// </summary>
		ResourceAccessFlags GetAccessFlags() const;

		/// <summary>
		/// Returns descriptor object for the texture
		/// </summary>
		const D3D11_TEXTURE2D_DESC& GetDescription() const;

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* GetResource() override;

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
		uint pixelStride;

		Texture2DBase();

		Texture2DBase(
			Device& dev,
			uivec2 dim,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceBindFlags bindFlags = ResourceBindFlags::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			uint mipLevels = 1u,
			uint arraySize = 1u,
			void* data = nullptr,
			uint pixelStride = 0u
		);
	};
}