#pragma once
#include "D3D11/dev/ResourceBase.hpp"
#include "D3D11/dev/Formats.hpp"

namespace Replica::D3D11
{
	class Texture2D : public ResourceBase
	{
	public:
		Texture2D(Device* pDev, 
			ivec2 dim, 
			void* data, 
			UINT stride, 
			Formats format = Formats::R8G8B8A8_UNORM, 
			UINT mipLevels = 1u
		);

		template<typename T>
		Texture2D(Device* pDev,
			ivec2 dim,
			IDynamicCollection<T> data,
			Formats format = Formats::R8G8B8A8_UNORM,
			UINT mipLevels = 1u
		) 
			: Texture2D(pDev, dim, data.GetPtr(), sizeof(T), format, mipLevels)
		{ }

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* GetResource() { return pRes.Get(); }

		/// <summary>
		/// Returns pointer to interface pointer field
		/// </summary>
		ID3D11Resource** GetResAddress() { return reinterpret_cast<ID3D11Resource**>(pRes.GetAddressOf()); }

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetSRV();

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView** GetSRVAddress();

		/// <summary>
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static Texture2D FromImageWIC(Device* pDev, const wchar_t* file);

	private:
		ComPtr<ID3D11Texture2D> pRes;
		ComPtr<ID3D11ShaderResourceView> pView;

	};
}