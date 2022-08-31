#pragma once
#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/InputLayout.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/ConstantMap.hpp"

namespace Replica::D3D11
{
	class Sampler;
	class Texture2D;
	class ConstantBuffer;
	class ConstantMap;
	class ConstantMapDef;

	class ShaderBase : public DeviceChild
	{
	public:
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
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(wstring_view name, const T& value)
		{
			constants.SetMember(name, value);
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat4>(wstring_view name, const mat4& value)
		{
			constants.SetMember(name, transpose(value));
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat3>(wstring_view name, const mat3& value)
		{
			constants.SetMember(name, transpose(value));
		}

		/// <summary>
		/// Sets sampler using last context
		/// </summary>
		virtual void SetSampler(Sampler& samp) = 0;

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		virtual void SetTexture(Texture2D& tex) = 0;

	protected:
		ConstantMap constants;
		ConstantBuffer cBuf;
		Context* pCtx;
		bool isBound;

		ShaderBase(Device& dev);

		ShaderBase(Device& dev, const ConstantMapDef& cDef);

		ShaderBase(ShaderBase&& other) noexcept;

		ShaderBase& operator=(ShaderBase&& other) noexcept;
	};
}
