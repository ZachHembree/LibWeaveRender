#pragma once
#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/InputLayout.hpp"

namespace Replica::D3D11
{ 
	class VertexShader : public Device::Child
	{
	public:
		VertexShader(const Device& dev, const LPCWSTR file, const std::initializer_list<IAElement>& layout);

		ID3D11VertexShader* Get() const;

		void Bind();

		const InputLayout& GetLayout() const;

	private:
		InputLayout layout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVS;
	};
}