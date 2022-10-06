#pragma once
#include "Texture2DBase.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Class representing a 2D Texture that can only be directly accessed by
	/// the CPU. GPU access is facilitated by copying to a non-staging texture.
	/// </summary>
	class StagingTexture2D : public Texture2DBase
	{
	public:
		using Texture2DBase::LoadImageWIC;

		/// <summary>
		/// Constructs an uninitialized texture object
		/// </summary>
		StagingTexture2D();

		/// <summary>
		/// Constructs and allocates a staging texture with the given format and size.
		/// CPU access flags cannot be set to none.
		/// </summary>
		StagingTexture2D(
			Device& dev,
			Formats format = Formats::R8G8B8A8_UNORM,
			ivec2 dim = ivec2(0),
			uint mipLevels = 1u,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::Write
		);

		/// <summary>
		/// Constructs and initializes a new staging texture with the given data
		/// </summary>
		template<typename T>
		StagingTexture2D(
			Device& dev,
			ivec2 dim,
			IDynamicArray<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			uint mipLevels = 1u,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::Write
		)
			: StagingTexture2D(dev, dim, data.GetPtr(), sizeof(T), format, accessFlags, mipLevels)
		{ }

		/// <summary>
		/// Constructs and initializes a new staging texture with the given data
		/// </summary>
		StagingTexture2D(
			Device& dev,
			ivec2 dim,
			void* data,
			uint stride,
			Formats format = Formats::R8G8B8A8_UNORM,
			uint mipLevels = 1u,
			ResourceAccessFlags accessFlags = ResourceAccessFlags::Write
		);

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
		void SetTextureData(Context& ctx, void* data, size_t stride, ivec2 dim);

	private:

	};
}