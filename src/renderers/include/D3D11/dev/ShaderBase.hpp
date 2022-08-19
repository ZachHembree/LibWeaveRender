#pragma once
#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/InputLayout.hpp"

namespace Replica::D3D11
{
	class Sampler;
	class Texture2D;
	class ConstantBuffer;

	class ShaderBase : public DeviceChild
	{
	public:
		/* To-Do
		void SetScalar(float v);

		void SetVector(vec2);

		void SetVector(vec4);

		void SetVector(vec4);

		void SetMatrix(mat4);
		*/

		/// <summary>
		/// Returns true if the shader is bound
		/// </summary>
		bool GetIsBound() { return isBound; }

		/// <summary>
		/// Binds shader to the main context
		/// </summary>
		virtual void Bind();

		/// <summary>
		/// Binds the shader to the given context
		/// </summary>
		virtual void Bind(Context& ctx) = 0;

		/// <summary>
		/// Unbinds the shader from the last context
		/// </summary>
		virtual void Unbind() = 0;

		/// <summary>
		/// Sets constant buffer using last context
		/// </summary>
		virtual void SetConstants(ConstantBuffer& cb) = 0;

		/// <summary>
		/// Sets sampler using last context
		/// </summary>
		virtual void SetSampler(Sampler& samp) = 0;

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		virtual void SetTexture(Texture2D& tex) = 0;

	protected:
		Context* pCtx;
		bool isBound;

		ShaderBase(Device& dev);

	};
}
