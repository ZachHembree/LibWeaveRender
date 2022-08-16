#pragma once
#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/InputLayout.hpp"

namespace Replica::D3D11
{ 
	class VertexShader : public DeviceChild
	{
	public:
		VertexShader(Device& dev, const LPCWSTR file, const std::initializer_list<IAElement>& layout);

		ID3D11VertexShader* Get() const;

		/// <summary>
		/// Binds the vertex shader to the given context
		/// </summary>
		void Bind(Context& ctx);

		/// <summary>
		/// Returns reference to vertex input layout
		/// </summary>
		const InputLayout& GetLayout() const;

	private:
		InputLayout layout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
	};
}