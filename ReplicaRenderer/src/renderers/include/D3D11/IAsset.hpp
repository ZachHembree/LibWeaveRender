#pragma once
#include "../D3DUtils.hpp"

namespace Replica::D3D11
{
	class Context;

	/// <summary>
	/// Interface for types used as resources by types of IDrawable
	/// </summary>
	class IAsset
	{
	public:
		virtual void Setup(Context& ctx) = 0;
	};
}