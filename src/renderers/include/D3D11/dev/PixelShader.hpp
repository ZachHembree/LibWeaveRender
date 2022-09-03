#pragma once
#include "D3D11/dev/ShaderBase.hpp"
#include "D3D11/ResourceMap.hpp"

namespace Replica::D3D11
{
	class Texture2D;
	class Sampler;

	struct PixelShaderDef : public ShaderDefBase
	{
		wstring_view file;
	};

	class PixelShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;

		PixelShader(Device& dev, const PixelShaderDef& psDef);

		ID3D11PixelShader* Get() const;

		/// <summary>
		/// Binds the pixel shader using the given context
		/// </summary>
		void Bind(Context& ctx) override;

		/// <summary>
		/// Unbinds pixel shader
		/// </summary>
		void Unbind() override;

		/// <summary>
		/// Sets sampler
		/// </summary>
		void SetSampler(wstring_view name, Sampler& samp) override;

		/// <summary>
		/// Sets Texture2D
		/// </summary>
		void SetTexture(wstring_view name, Texture2D& tex) override;

	private:
		ComPtr<ID3D11PixelShader> pPS;

	};
}