#pragma once
#include "Texture2DBase.hpp"

namespace Weave::D3D11
{
	class StagingTexture2D;
	class Tex2DBufferHandle;
	class Context;

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
			ResourceAccessFlags accessFlags = ResourceAccessFlags::ReadWrite
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
			ResourceAccessFlags accessFlags = ResourceAccessFlags::ReadWrite
		)
			: StagingTexture2D(dev, dim, data.GetData(), sizeof(T), format, accessFlags, mipLevels)
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
			ResourceAccessFlags accessFlags = ResourceAccessFlags::ReadWrite
		);

		/// <summary>
		/// Updates texture with contents of a scratch image, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureWIC(ContextBase& ctx, wstring_view file, DirectX::ScratchImage& buffer);

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		template<typename T>
		void SetTextureData(ContextBase& ctx, const IDynamicArray<T>& src, ivec2 dim)
		{
			Span<byte> srcBytes(reinterpret_cast<byte*>(src.GetData()), GetArrSize(src));
			SetTextureData(ctx, srcBytes, sizeof(T), dim);
		}

		/// <summary>
		/// Updates texture with contents of an arbitrary pixel data buffer, assuming compatible formats.
		/// Allocates new Texture2D if the dimensions aren't the same.
		/// </summary>
		void SetTextureData(ContextBase& ctx, const IDynamicArray<byte>& src, uint pixStride, ivec2 dim);

		/// <summary>
		/// Returns a temporary handle to a mapped buffer backing the texture
		/// </summary>
		Tex2DBufferHandle GetBufferHandle(Context& ctx);

		/// <summary>
		/// Returns and unmaps a previously acquired buffer handle accessed using GetBufferHandle()
		/// </summary>
		void ReturnBufferHandle(Context& ctx, Tex2DBufferHandle&& handle);

		/// <summary>
		/// Writes the contents of the texture to a PNG in the give file path. Requires
		/// CPU read access.
		/// </summary>
		void WriteToFileWIC(Context& ctx, string_view file);

		/// <summary>
		/// Writes the contents of the texture to a PNG in the give file path. Requires
		/// CPU read access.
		/// </summary>
		void WriteToFileWIC(Context& ctx, wstring_view file);

	private:

	};

	/// <summary>
	/// Provides temporary access to a mapped resource buffer. Must be returned to parent
	/// after use.
	/// </summary>
	class Tex2DBufferHandle : public DataBufferSpan<StagingTexture2D, byte>
	{
	friend StagingTexture2D;

	public:
		/// <summary>
		/// True if the underlying texture buffer supports CPU read access
		/// </summary>
		const bool canRead;

		/// <summary>
		/// True if the underlying texture buffer supports CPU write access
		/// </summary>
		const bool canWrite;

		Tex2DBufferHandle();

		/// <summary>
		/// Returns the number of bytes in a row
		/// </summary>
		size_t GetRowPitch() const;

		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		size_t GetByteSize() const;

	private:
		const D3D11_MAPPED_SUBRESOURCE msr;

		Tex2DBufferHandle(StagingTexture2D* pParent, D3D11_MAPPED_SUBRESOURCE msr);

	};
}