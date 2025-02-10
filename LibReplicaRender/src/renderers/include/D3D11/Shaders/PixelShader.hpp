#pragma once
#include "ShaderBase.hpp"

namespace Replica::D3D11
{
	class Texture2D;
	class Sampler;

	class PixelShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;
		using ShaderBase::SetSampler;
		using ShaderBase::SetTexture;

		PixelShader();

		PixelShader(Device& dev, const ShaderDef& psDef);

		ID3D11PixelShader* Get() const;

		/// <summary>
		/// Binds the pixel shader using the given context
		/// </summary>
		void Bind(Context& ctx) override;

		/// <summary>
		/// Unbinds pixel shader
		/// </summary>
		void Unbind() override;

	private:
		ComPtr<ID3D11PixelShader> pPS;

	};
}