#pragma once
#include "ReplicaMath.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Defines the bounds of a viewport rendered to by the pipeline.
	/// Effects using multiple render targets can specify multiple viewports
	/// before the back buffer is rendered to.
	/// </summary>
	struct Viewport
	{
		/// <summary>
		/// Offset in pixels from the top left corner. 
		/// Usually (0, 0).
		/// </summary>
		vec2 offset;

		/// <summary>
		/// Defines the resolution of the region rendered in pixels.
		/// </summary>
		vec2 size;

		/// <summary>
		/// Normalized range of depth values included in the output.
		/// Usually on (0, 1].
		/// </summary>
		vec2 zDepth;

	};
}