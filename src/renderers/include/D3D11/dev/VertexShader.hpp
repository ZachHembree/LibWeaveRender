#pragma once
#include "D3D11/dev/ShaderBase.hpp"
#include "D3D11/dev/InputLayout.hpp"

namespace Replica::D3D11
{ 
	class VertexShader : public ShaderBase
	{
	public:
		using ShaderBase::Bind;

		VertexShader(
			Device& dev,
			wstring_view file,
			const IDynamicCollection<IAElement>& layout,
			const ConstantMapDef& cDef
		);

		VertexShader(
			Device& dev, 
			wstring_view file,
			const std::initializer_list<IAElement>& layout,
			const ConstantMapDef& cDef
		) : 
			VertexShader(dev, file, UniqueArray(layout), cDef)
		{ };

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
		/// Sets sampler using last context
		/// </summary>
		void SetSampler(Sampler& samp) override;

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		void SetTexture(Texture2D& tex) override;

	private:
		InputLayout layout;
		ComPtr<ID3D11VertexShader> pVS;
	};
}