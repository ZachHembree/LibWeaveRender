#pragma once
#include "ContextBase.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Device context for issuing commands to an associated D3D11 device
	/// </summary>
	class Context : public ContextBase
	{
	public:
		Context();

		Context(Device& dev, ComPtr<ID3D11DeviceContext>&& pContext);

		Context(Context&&) noexcept;

		Context& operator=(Context&&) noexcept;

		~Context();

		/// <summary>
		/// Returns reference to context interface
		/// </summary>
		ID3D11DeviceContext& Get() const;
		
		/// <summary>
		/// Returns reference to context interface
		/// </summary>
		ID3D11DeviceContext* operator->() const;

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

		/// <summary>
		/// Draws an indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(Mesh& mesh, Material& mat);

		/// <summary>
		/// Draws a group of indexed, non-instanced triangle meshes using the given material
		/// </summary>
		void Draw(IDynamicArray<Mesh>& meshes, Material& mat);

	};
}