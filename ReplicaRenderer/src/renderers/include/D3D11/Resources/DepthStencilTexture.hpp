#pragma once
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Resources/TextureEnums.hpp"

namespace Replica::D3D11
{
	enum class DSClearFlags : int
	{
		None = 0,
		Depth = D3D11_CLEAR_DEPTH,
		Stencil = D3D11_CLEAR_STENCIL,
		DepthStencil = Depth | Stencil
	};

	BITWISE_ALL(DSClearFlags, int)

	class IDepthStencil
	{
	public:
		/// <summary>
		/// Returns pointer to depth stencil state interface
		/// </summary>
		virtual ID3D11DepthStencilState* GetState() = 0;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		virtual ID3D11DepthStencilView* GetDSV() = 0;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		virtual ID3D11DepthStencilView** const GetDSVAddress() = 0;

		/// <summary>
		/// Clears the texture
		/// </summary>
		virtual void Clear(
			Context& ctx,
			DSClearFlags clearFlags = DSClearFlags::Depth,
			float depthClear = 1.0f,
			UINT8 stencilClear = 0
		) const = 0;
	};

	class DepthStencilTexture : public Texture2D, public IDepthStencil
	{
	public:
		DepthStencilTexture();

		DepthStencilTexture(
			Device& dev,
			ivec2 dim,
			Formats format = Formats::D32_FLOAT,
			ResourceUsages usage = ResourceUsages::Default,
			TexCmpFunc depthCmp = TexCmpFunc::LESS
		);

		/// <summary>
		/// Returns pointer to depth stencil state interface
		/// </summary>
		ID3D11DepthStencilState* GetState() override;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		ID3D11DepthStencilView* GetDSV() override;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		ID3D11DepthStencilView** const GetDSVAddress() override;

		/// <summary>
		/// Clears the texture
		/// </summary>
		void Clear(
			Context& ctx,
			DSClearFlags clearFlags = DSClearFlags::Depth,
			float depthClear = 1.0f,
			UINT8 stencilClear = 0
		) const override;

	private:
		ComPtr<ID3D11DepthStencilState> pState;
		ComPtr<ID3D11DepthStencilView> pDSV;
	};
}