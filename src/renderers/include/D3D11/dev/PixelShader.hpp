#pragma once
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	class PixelShader : public DeviceChild
	{
	public:
		PixelShader(Device& dev, const LPCWSTR file);

		ID3D11PixelShader* Get() const;

		/// <summary>
		/// Binds the pixel shader using the given context
		/// </summary>
		void Bind(Context& ctx);

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
	};
}