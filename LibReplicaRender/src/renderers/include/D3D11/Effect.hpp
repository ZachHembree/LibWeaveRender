#pragma once
#include "IAsset.hpp"
#include "ShaderLibrary.hpp"

namespace Replica::D3D11
{
	class Effect : public IAsset
	{
	public:
		Effect() : pLib(nullptr), pDef(nullptr) {}

		Effect(
			ShaderLibrary& lib,
			const EffectDef& effectDef
		) :
			pLib(&lib),
			pDef(&effectDef)
		{ }

		string_view GetName() const { return pDef->name; }

		uint GetVariantID() const { return pDef->variantID; }

		const IDynamicArray<int>& GetShaderIDs(int pass) const { return pDef->passes[pass].shaderIDs; }

		int GetPassCount() const { return pDef->passes.GetLength(); }

		ShaderBase& GetShader(int shaderID) { return pLib->GetShader(shaderID, GetVariantID()); }

		/// <summary>
		/// Sets the value corresponding to the given name to the
		/// given value.
		/// </summary>
		template<typename T>
		void SetConstant(string_view name, const T& value, int pass = 0)
		{
			for (int shaderID : GetShaderIDs(pass))
				GetShader(shaderID).SetConstant(name, value);
		}

		/// <summary>
		/// Sets sampler 
		/// </summary>
		void SetSampler(string_view name, Sampler& samp, int pass = 0)
		{
			for (int shaderID : GetShaderIDs(pass))
				GetShader(shaderID).SetSampler(name, samp);
		}

		/// <summary>
		/// Sets Texture2D
		/// </summary>
		void SetTexture(string_view name, ITexture2D& tex, int pass = 0)
		{
			for (int shaderID : GetShaderIDs(pass))
				GetShader(shaderID).SetTexture(name, tex);
		}

		void Setup(Context& ctx) override // To-Do: multi-pass support
		{
			for (int shaderID : GetShaderIDs(0))
				GetShader(shaderID).Bind(ctx);
		}

		void Reset(int pass = 0)
		{
			for (int shaderID : GetShaderIDs(pass))
				GetShader(shaderID).Unbind();
		}

	private:
		ShaderLibrary* pLib;
		const EffectDef* pDef;
	};
}
