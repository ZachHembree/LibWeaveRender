#pragma once

namespace Weave::D3D11
{
	class Context;

	/// <summary>
	/// Interface for types used as resources by types of IDrawable
	/// </summary>
	class IAsset
	{
	public:
		virtual void Setup(ContextBase& ctx) = 0;
	};
}