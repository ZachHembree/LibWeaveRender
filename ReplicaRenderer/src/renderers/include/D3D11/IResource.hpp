#pragma once
#include "D3D11/D3DUtils.hpp"

namespace Replica::D3D11
{
	class Context;

	/// <summary>
	/// Interface for types used as resources by types of IDrawable
	/// </summary>
	class IResource
	{
	public:
		virtual void Update(Context& ctx) = 0;
	};
}