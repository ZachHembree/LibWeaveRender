#pragma once
#include "D3D11/Shaders/ShaderBase.hpp"
#include "D3D11/Resources/InputLayout.hpp"
#include "D3D11/ResourceMap.hpp"

namespace Replica::D3D11
{ 
	struct VertexShaderDef : public ShaderDefBase
	{
		wstring_view file;
		DynamicArray<IAElement> iaLayout;
	};

	class VertexShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;

		VertexShader(
			Device& dev,
			const VertexShaderDef& vsDef
		);

		VertexShader(VertexShader&& other) noexcept;

		VertexShader& operator=(VertexShader&& other) noexcept;

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

		/// <summary>
		/// Sets sampler
		/// </summary>
		void SetSampler(wstring_view name, Sampler& samp) override;

		/// <summary>
		/// Sets Texture2D
		/// </summary>
		void SetTexture(wstring_view name, Texture2D& tex) override;

	private:
		InputLayout layout;
		ComPtr<ID3D11VertexShader> pVS;
	};
}