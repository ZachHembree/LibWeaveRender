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
		/*void SetScalar(const wchar_t* name, float v);

		void SetVector(const wchar_t* name, vec2 v);

		void SetVector(const wchar_t* name, vec3 v);

		void SetVector(const wchar_t* name, vec4 v);

		void SetScalar(const wchar_t* name, int v);

		void SetVector(const wchar_t* name, ivec2 v);

		void SetVector(const wchar_t* name, ivec3 v);

		void SetVector(const wchar_t* name, ivec4 v);

		void SetMatrix(const wchar_t* name, mat4 v);*/

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
