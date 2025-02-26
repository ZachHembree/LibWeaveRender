#pragma once
#include "ShaderBase.hpp"

namespace Replica::D3D11
{
	class PixelShader : public ShaderInstance<ID3D11PixelShader>
	{
	public:
		using ShaderInstance::SetSampler;
		using ShaderInstance::SetTexture;

		PixelShader();

		PixelShader(const PixelShaderVariant& ps);

		/// <summary>
		/// Binds the pixel shader using the given context
		/// </summary>
		void Bind(Context& ctx) override;

		/// <summary>
		/// Unbinds pixel shader
		/// </summary>
		void Unbind(Context& ctx) override;

		void UnmapResources(Context& ctx) override;

	private:
	};
}