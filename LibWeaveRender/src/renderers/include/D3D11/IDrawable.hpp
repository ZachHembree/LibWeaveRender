#pragma once
#include "IAsset.hpp"

namespace Weave::D3D11
{
	class CtxBase;
	class EffectVariant;
	class Material;

	/// <summary>
	/// Interface for types that can be drawn
	/// </summary>
	class IDrawable : public IAsset
	{
	public:
		virtual void Draw(CtxBase& ctx, Material& mat) = 0;
	};
}