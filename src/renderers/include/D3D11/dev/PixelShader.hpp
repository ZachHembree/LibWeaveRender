#pragma once
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	class PixelShader : public Device::Child
	{
	public:
		PixelShader(const Device& dev, const LPCWSTR file);

		ID3D11PixelShader* Get() const;

		/// <summary>
		/// Binds the pixel shader to the context associated with the device
		/// </summary>
		void Bind();

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
	};
}