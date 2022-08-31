#pragma once
#include "D3D11/dev/ShaderBase.hpp"

namespace Replica::D3D11
{
	class Texture2D;
	class Sampler;

	class PixelShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;

		PixelShader(Device& dev, const wstring_view file, const ConstantMapDef& cDefPS);

		PixelShader(PixelShader&& other) noexcept :
			ShaderBase(std::move(other)),
			pPS(std::move(other.pPS))
		{ }

		PixelShader& operator=(PixelShader&& other) noexcept
		{
			ShaderBase::operator=(std::move(other));
			this->pPS = std::move(other.pPS);

			return *this;
		}

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
		/// Sets sampler using last context
		/// </summary>
		void SetSampler(Sampler& samp) override;

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		void SetTexture(Texture2D& tex) override;

	private:
		ComPtr<ID3D11PixelShader> pPS;

	};
}