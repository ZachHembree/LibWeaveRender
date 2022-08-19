#pragma once
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	class Texture2D;

	class PixelShader : public DeviceChild
	{
	public:
		PixelShader(Device& dev, const LPCWSTR file);

		ID3D11PixelShader* Get() const;

		void SetScalar(float v);

		template<typename T>
		void SetVector(tvec4<T>);

		void SetMatrix(mat4);

		void SetTexture(Texture2D& tex);

		/// <summary>
		/// Binds the pixel shader using the given context
		/// </summary>
		void Bind(Context& ctx);

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPS;
	};
}