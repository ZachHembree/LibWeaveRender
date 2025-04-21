#pragma once

namespace Weave::D3D11
{
	class CtxBase;

	/// <summary>
	/// Interface for types used as resources by types of IDrawable
	/// </summary>
	class IAsset
	{
	public:
		virtual void Setup(CtxBase& ctx) = 0;
	};
}