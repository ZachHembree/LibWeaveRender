#pragma once
#include "IAsset.hpp"

namespace Replica::D3D11
{
	class Context;
	class EffectVariant;

	/// <summary>
	/// Interface for types that can be drawn
	/// </summary>
	class IDrawable : public IAsset
	{
	public:
		virtual void Draw(Context& ctx, EffectVariant& effect) = 0;
	};
}