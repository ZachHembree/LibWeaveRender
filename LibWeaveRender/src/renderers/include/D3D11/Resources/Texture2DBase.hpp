#pragma once
#include "WeaveUtils/Span.hpp"
#include "ResourceBase.hpp"
#include "../D3D11Utils.hpp"

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
		const Texture2DDesc& GetDescription() const;

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* GetResource() const override;

		/// <summary>
		/// Returns pointer to interface pointer field
		/// </summary>
		ID3D11Resource* const* GetResAddress() const override;

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		template<typename T>
		void SetTextureData(CtxBase& ctx, const IDynamicArray<T>& src, uivec2 dim)
		{
			Span<byte> srcBytes(reinterpret_cast<byte*>(src.GetData()), GetArrSize(src));
			SetTextureData(ctx, srcBytes, sizeof(T), dim);
		}

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		virtual void SetTextureData(CtxBase& ctx, const IDynamicArray<byte>& src, uint pixStride, uivec2 dim);

		/// <summary>
		/// Loads WIC-compatible image into a buffer
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static void LoadImageWIC(wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Returns true if the object is valid and has been initialized
		/// </summary>
		bool GetIsValid() const override;

		/// <summary>
		/// Allocates a new texture with the given dimensions and resets the contents
		/// </summary>
		void Reset(uivec2 dim = uivec2(-1), Formats newFormat = Formats::UNKNOWN);

	protected:
		DECL_MOVE_ONLY(Texture2DBase);

		UniqueComPtr<ID3D11Texture2D> pRes;
		Texture2DDesc desc;
		uint pixelStride;

		Texture2DBase();

		Texture2DBase(
			Device& dev,
			uivec2 dim = uivec2(0),
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceBindFlags bindFlags = ResourceBindFlags::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			uint mipLevels = 1u,
			uint arraySize = 1u,
			void* data = nullptr,
			uint pixelStride = 0u
		);

		virtual ~Texture2DBase();

		virtual void Init() = 0;
	};
}