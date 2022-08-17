#pragma once
#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/Formats.hpp"

namespace Replica::D3D11
{
	class Texture2D : public DeviceChild
	{
	public:
		Texture2D(Device* pDev, 
			ivec2 dim, 
			void* data, 
			UINT stride, 
			Formats format = Formats::R8G8B8A8_UNORM, 
			UINT mipLevels = 1u
		);

		Texture2D(Device* pDev,
			const wchar_t* file
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

		Texture2D(Texture2D&& other) noexcept;

		Texture2D& operator=(Texture2D&& other) noexcept;

		/// <summary>
		/// Returns interface to resource
		/// </summary>
		ID3D11Resource* Get();

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetView();

		/// <summary>
		/// Binds the texture to the pixel shader stage
		/// </summary>
		void Bind(Context& ctx, UINT slot = 0u);

	private:
		ComPtr<ID3D11Texture2D> pRes;
		ComPtr<ID3D11ShaderResourceView> pView;
	};
}