#pragma once
#include "ShaderInstanceBase.hpp"

namespace Weave::D3D11
{
	class ShaderVariantManager;
	class EffectVariant;
	class ResourceSet;

	class Material : public ShaderInstanceBase
	{
	public:		
		DECL_DEST_MOVE(Material)

		Material();

		Material(ShaderVariantManager& lib, uint nameID, uint vID);

		/// <summary>
		/// Returns the number of passes in the effect
		/// </summary>
		uint GetPassCount() const;

		/// <summary>
		/// Returns the number of shaders specified for a given pass
		/// </summary>
		uint GetShaderCount(int pass) const;

		/// <summary>
		/// Prepares shaders and resources for the given pass before a draw call
		/// </summary>
		void Setup(CtxBase& ctx, int pass);

	private:
		mutable const EffectVariant* pEffect;

		const EffectVariant& GetEffect() const;

		void SetVariantID(uint vID) override;
	};
}