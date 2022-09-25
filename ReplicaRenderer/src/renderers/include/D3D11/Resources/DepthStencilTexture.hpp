#pragma once
#include "Texture2D.hpp"
#include "TextureEnums.hpp"

namespace Replica::D3D11
{
	class DepthStencilTexture : public Texture2DBase, public IDepthStencil
	{
	public:
		DepthStencilTexture();

		DepthStencilTexture(
			Device& dev,
			ivec2 dim,
			vec2 range = vec2(0, 1),
			Formats format = Formats::D32_FLOAT,
			ResourceUsages usage = ResourceUsages::Default,
			TexCmpFunc depthCmp = TexCmpFunc::LESS
		);

		/// <summary>
		/// Sets the acceptable range of normalized values for objects rendered
		/// with this depth buffer
		/// </summary>
		void SetRange(vec2 range);

		/// <summary>
		/// Returns the range of normalized depth values accepted by the buffer
		/// </summary>
		vec2 GetRange() const override;

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
		vec2 range;
	};
}