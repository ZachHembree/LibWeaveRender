#pragma once
#include "ShaderVariantBase.hpp"
#include "../Resources/DeviceChild.hpp"
#include "../Resources/InputLayout.hpp"
#include "../Resources/ConstantBuffer.hpp"
#include "../Resources/ConstantMap.hpp"
#include "../ResourceMap.hpp"

namespace Replica::D3D11
{
	class Sampler;
	class Texture2D;
	class ConstantBuffer;
	class ConstantMap;

	class ShaderBase
	{
	public:
		/// <summary>
		/// Binds the shader to the given context
		/// </summary>
		virtual void Bind(Context& ctx) = 0;

		virtual void Unbind(Context& ctx) = 0;

		virtual void UnmapResources(Context& ctx) = 0;

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		void SetBuffer(string_view name, const byte* pSrc, const size_t size);

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		void SetConstant(string_view name, const byte* pSrc, const size_t size);

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(string_view name, const T& value) { SetConstant(name, (byte*)&value, sizeof(T)); }

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat4>(string_view name, const mat4& value)
		{
			const mat4 x = transpose(value);
			SetConstant(name, (byte*)&x, sizeof(mat4));
		}

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<>
		void SetConstant<mat3>(string_view name, const mat3& value)
		{
			const mat3 x = transpose(value);
			SetConstant(name, (byte*)&x, sizeof(mat3));
		}

		/// <summary>
		/// Sets sampler using last context
		/// </summary>
		virtual void SetSampler(string_view name, Sampler& samp);

		/// <summary>
		/// Sets Texture2D using last context
		/// </summary>
		virtual void SetTexture(string_view name, ITexture2D& tex);

	protected:
		MAKE_NO_COPY(ShaderBase)

		std::unordered_map<string_view, ConstantMap> constants;
		ResourceMap<ID3D11SamplerState> samplers;
		ResourceMap<ID3D11ShaderResourceView> textures;

		UniqueArray<ConstantBuffer> cBuffers; // Temporary

		ShaderBase();

		ShaderBase(Device& dev, const ShaderDef& def);

		ShaderBase(ShaderBase&& other) noexcept;

		ShaderBase& operator=(ShaderBase&& other) noexcept;

		void UpdateConstants(Context& ctx);
	};

	template<typename ShaderT>
	class ShaderInstance : public ShaderBase
	{
	public:
		using ShaderVariantT = ShaderVariant<ShaderT>;

		ShaderT* Get() const { return pShader->Get(); }

		void SetVariant(const int vID) { pShader = &pShader->GetVariant(vID); }

	protected:
		const ShaderVariantT* pShader;

		ShaderInstance() : 
			ShaderBase(),
			pShader(nullptr)
		{ }

		ShaderInstance(const ShaderVariantT& variant) : 
			ShaderBase(variant.GetDevice(), variant.GetDefinition()),
			pShader(&variant)
		{ }
	};
}
