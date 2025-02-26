#pragma once
#include "ShaderBase.hpp"
#include "../Resources/InputLayout.hpp"

namespace Replica::D3D11
{ 
	class VertexShader : public ShaderInstance<ID3D11VertexShader>
	{
	public:
		using ShaderInstance::Bind;
		using ShaderInstance::SetSampler;
		using ShaderInstance::SetTexture;

		VertexShader();

		VertexShader(const VertexShaderVariant& vs);

		/// <summary>
		/// Returns reference to vertex input layout
		/// </summary>
		const InputLayout& GetLayout() const;

		/// <summary>
		/// Binds the vertex shader to the given context
		/// </summary>
		void Bind(Context& ctx) override;

		void Unbind(Context& ctx) override;

		void UnmapResources(Context& ctx) override;

	private:
		InputLayout layout;
	};
}