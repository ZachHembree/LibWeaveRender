#pragma once
#include "ShaderBase.hpp"
#include "../Resources/InputLayout.hpp"

namespace Replica::D3D11
{ 
	struct VertexShaderDef : public ShaderDefBase
	{
		DynamicArray<IAElement> iaLayout;
	};

	class VertexShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;
		using ShaderBase::SetSampler;
		using ShaderBase::SetTexture;

		VertexShader();

		VertexShader(Device& dev, const VertexShaderDef& vsDef );

		ID3D11VertexShader* Get() const;

		/// <summary>
		/// Returns reference to vertex input layout
		/// </summary>
		const InputLayout& GetLayout() const;

		/// <summary>
		/// Binds the vertex shader to the given context
		/// </summary>
		void Bind(Context& ctx) override;

		/// <summary>
		/// Unbinds vertex shader;
		/// </summary>
		void Unbind() override;

	private:
		InputLayout layout;
		ComPtr<ID3D11VertexShader> pVS;
	};
}