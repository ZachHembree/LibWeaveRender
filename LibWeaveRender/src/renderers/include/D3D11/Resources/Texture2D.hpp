#pragma once
#include "WeaveUtils/Span.hpp"
#include "Texture2DBase.hpp"

namespace DirectX
{
	class ScratchImage;
}

namespace Weave::D3D11
{
	/// <summary>
	/// Class representing a read-only GPU resource. Can be updated by CPU.
	/// </summary>
	class Texture2D : public virtual ITexture2D, public Texture2DBase
	{
	public:
		DECL_DEST_MOVE(Texture2D);

		/// <summary>
		/// Constructs and alocates a texture initialized with the given data with either
		/// Default or Immutable usage, depending whether the read-only flag is set.
		/// </summary>
		template<typename T>
		Texture2D(
			Device& dev,
			uivec2 dim,
			IDynamicArray<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			uint mipLevels = 1u,
			bool isReadonly = true
		)
			: Texture2D(dev, dim, data.GetData(), sizeof(T), format, isReadonly, mipLevels)
		{ }

		/// <summary>
		/// Constructs and alocates a texture initialized with the given data with either
		/// Default or Immutable usage, depending whether the read-only flag is set.
		/// </summary>
		Texture2D(
			Device& dev,
			uivec2 dim,
			void* data,
			uint stride,
			Formats format = Formats::R8G8B8A8_UNORM,
			uint mipLevels = 1u,
			bool isReadonly = true
		);

		/// <summary>
		/// Constructs and alocates an empty texture with default or dynamic usage.
		/// </summary>
		Texture2D(
			Device& dev,
			Formats format = Formats::R8G8B8A8_UNORM,
			uivec2 dim = uivec2(0),
			uint mipLevels = 1u,
			bool isDynamic = false
		);

		/// <summary>
		/// Constructs an uninitialized texture object
		/// </summary>
		Texture2D();

		/// <summary>
		/// Returns true if the texture is immutable
		/// </summary>
		bool GetIsReadOnly() const;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetSRV() const override;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* const* GetSRVAddress() const override;

		/// <summary>
		/// Updates texture with contents of a scratch image, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureWIC(CtxBase& ctx, wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static Texture2D FromImageWIC(
			Device& dev,
			wstring_view file,
			bool isReadOnly = true
		);
		
	protected:
		UniqueComPtr<ID3D11ShaderResourceView> pSRV;

		Texture2D(
			Device& dev,
			uivec2 dim,
			Formats format = Formats::R8G8B8A8_UNORM,
			ResourceUsages usage = ResourceUsages::Default,
			ResourceBindFlags bindFlags = ResourceBindFlags::ShaderResource,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::None,
			uint mipLevels = 1u,
			void* data = nullptr,
			uint stride = 0
		);

		void Init() override;
	};
}