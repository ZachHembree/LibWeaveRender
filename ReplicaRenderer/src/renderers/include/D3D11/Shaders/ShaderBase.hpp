#pragma once
#include "D3D11/Resources/DeviceChild.hpp"
#include "D3D11/Resources/InputLayout.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/ConstantMap.hpp"
#include "D3D11/ResourceMap.hpp"

namespace Replica::D3D11
{
	class Sampler;
	class Texture2D;
	class ConstantBuffer;
	class ConstantMap;
	class ConstantMapDef;

	struct ShaderDefBase
	{
		ConstantMapDef constMap;
		ResourceMap<ID3D11SamplerState> samplerMap;
		ResourceMap<ID3D11ShaderResourceView> textureMap;
	};

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
		virtual void SetSampler(wstring_view name, Sampler& samp) = 0;

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		virtual void SetTexture(wstring_view name, Texture2D& tex) = 0;

	protected:
		ConstantMap constants;
		ResourceMap<ID3D11SamplerState> samplers;
		ResourceMap<ID3D11ShaderResourceView> textures;

		ConstantBuffer cBuf;
		Context* pCtx;
		bool isBound;

		ShaderBase();

		ShaderBase(Device& dev);

		ShaderBase(Device& dev, const ShaderDefBase& def);

		ShaderBase(ShaderBase&& other) noexcept;

		ShaderBase& operator=(ShaderBase&& other) noexcept;
	};
}
