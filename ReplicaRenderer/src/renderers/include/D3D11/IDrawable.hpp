#pragma once
#include "D3D11/D3DUtils.hpp"

namespace Replica::D3D11
{
	class Context;

	/// <summary>
	/// Interface for types that can be drawn
	/// </summary>
	class IDrawable
	{
	public:
		virtual void Setup(Context& ctx) = 0;

		virtual void Draw(Context& ctx) = 0;
	};
}