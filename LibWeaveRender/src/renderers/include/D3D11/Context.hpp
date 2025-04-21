#pragma once
#include "ContextBase.hpp"
#include "Resources/MappedBufferHandle.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Device context for issuing commands to an associated D3D11 device
	/// </summary>
	class Context : public ContextBase
	{
	public:
		Context();

		Context(Device& dev, ComPtr<ID3D11DeviceContext>&& pCtx);

		Context(Context&&) noexcept;

		Context& operator=(Context&&) noexcept;

		~Context();

		ID3D11DeviceContext* Get();

		/// <summary>
		/// Returns a handle for reading and or writing the given buffer, based on the 
		/// buffer's usage type.
		/// </summary>
		MappedBufferHandle GetMappedBufferHandle(IBuffer& buffer);

		/// <summary>
		/// Returns the buffer handle and unmaps it from the CPU.
		/// </summary>
		void ReturnMappedBufferHandle(MappedBufferHandle&& handle);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, IRWTexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IRWTexture2D& dst, ivec4 srcBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2D& src, IResizeableTexture2D& dst, ivec4 srcBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, ITexture2D& dst, ivec4 dstBox = ivec4(0));

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(IResizeableTexture2D& src, IResizeableTexture2D& dst);

		/// <summary>
		/// Copies the contents of one texture to another
		/// </summary>
		void Blit(ITexture2DBase& src, ITexture2DBase& dst, ivec4 srcBox = ivec4(0), ivec4 dstBox = ivec4(0));

		/// <summary>
		/// Copies the contents of a texture to a render target
		/// </summary>
		void Blit(IResizeableTexture2D& src, IRenderTarget& dst);

		/// <summary>
		/// Copies the contents of a texture to a render target
		/// </summary>
		void Blit(ITexture2D& src, IRenderTarget& dst, ivec4 srcBox = ivec4(0));
	};
}