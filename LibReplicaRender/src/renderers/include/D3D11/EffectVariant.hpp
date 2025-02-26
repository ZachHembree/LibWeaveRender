#pragma once
#include "IAsset.hpp"
#include "ShaderLibGen/ShaderData.hpp"
#include "../D3D11/Resources/ResourceBase.hpp"
#include "../D3D11/Shaders/ShaderBase.hpp"

namespace Replica::D3D11
{
	using Effects::EffectDef;

	class Sampler;
	class ShaderLibrary;

	class EffectVariant
	{
	public:
		MAKE_MOVE_ONLY(EffectVariant)

		EffectVariant();

		EffectVariant(
			ShaderLibrary& lib,
			const EffectDef& effectDef
		);

		/// <summary>
		/// Returns the name of the effect
		/// </summary>
		string_view GetName() const;

		/// <summary>
		/// Returns the unique identifier for the effect
		/// </summary>
		uint GetVariantID() const;

		/// <summary>
		/// Returns the library that owns the effect
		/// </summary>
		const ShaderLibrary& GetLibrary() const;

		/// <summary>
		/// Returns the number of passes in the effect
		/// </summary>
		/// <returns></returns>
		int GetPassCount() const;

		/// <summary>
		/// Returns the number of shaders in the given pass
		/// </summary>
		int GetShaderCount(const int pass) const;

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(string_view name, const T& value)
		{
			for (int i = 0; i < GetPassCount(); i++)
			{
				for (int j = 0; j < GetShaderCount(i); j++)
					GetShader(j, i).SetConstant(name, value);
			}
		}

		/// <summary>
		/// Assigns sampler with the given name
		/// </summary>
		void SetSampler(string_view name, Sampler& samp);

		/// <summary>
		/// Assigns a Texture2D with the given name
		/// </summary>
		void SetTexture(string_view name, ITexture2D& tex);

		void Setup(Context& ctx, int pass = 0);

		void Reset(Context& ctx, int pass = 0);

	private:
		using ShaderHandle = std::unique_ptr<ShaderBase>;
		using EffectPass = UniqueArray<ShaderHandle>;
		
		ShaderLibrary* pLib;
		const EffectDef* pDef;
		UniqueArray<EffectPass> passes;

		ShaderBase& GetShader(int index, int pass);
	};
}
