#pragma once
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/Formats.hpp"

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
			UINT mipLevels = 1u
		)
			: Texture2D(dev, dim, data.GetPtr(), sizeof(T), format, mipLevels)
		{ }

		Texture2D(
			Device& dev,
			ivec2 dim, 
			void* data, 
			UINT stride, 
			Formats format = Formats::R8G8B8A8_UNORM, 
			UINT mipLevels = 1u
		);

		Texture2D();

		ivec2 GetSize() const;

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
		/// Initializes new Texture2D from WIC-compatible image 
		/// (BMP, GIF, ICO, JPEG, PNG, TIFF)
		/// </summary>
		static Texture2D FromImageWIC(Device& dev, const wchar_t* file);

	protected:
		ComPtr<ID3D11Texture2D> pRes;
		ComPtr<ID3D11ShaderResourceView> pRTV;
		ivec2 size;

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