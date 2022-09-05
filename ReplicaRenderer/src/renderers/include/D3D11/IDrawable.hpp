#pragma once
#include "D3D11/IResource.hpp"

namespace Replica::D3D11
{
	class Context;

	/// <summary>
	/// Interface for types that can be drawn
	/// </summary>
	class IDrawable : public IResource
	{
	public:
		virtual void Draw(Context& ctx) = 0;
	};
}