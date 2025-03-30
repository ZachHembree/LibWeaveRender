#pragma once
#include "ShaderDataHandles.hpp"
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/ResourceSet.hpp"

namespace Replica::D3D11
{
	using Effects::EffectDef;
	using Effects::EffectPass;

	class Sampler;
	class ShaderLibrary;
	class ShaderVariantBase;

	class EffectVariant
	{
	public:
		MAKE_MOVE_ONLY(EffectVariant)

		EffectVariant();

		EffectVariant(
			ShaderLibrary& lib,
			EffectDefHandle effectDef
		);

		/// <summary>
		/// Returns the name ID of the effect
		/// </summary>
		uint GetNameID() const;

		/// <summary>
		/// Returns the number of passes in the effect
		/// </summary>
		uint GetPassCount() const;

		/// <summary>
		/// Returns the number of shaders in the given pass
		/// </summary>
		uint GetShaderCount(const int pass) const;

		void Setup(Context& ctx, int pass, const ResourceSet& res) const;

		void Reset(Context& ctx, int pass) const;

	private:
		using PassHandle = UniqueArray<const ShaderVariantBase*>;

		EffectDefHandle def;
		UniqueArray<PassHandle> passes;
	};
}
