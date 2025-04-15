#pragma once
#include "WeaveEffects/ShaderLibMap.hpp"
#include "Shaders/Material.hpp"
#include "Shaders/ComputeInstance.hpp"

namespace Weave::D3D11
{
	using Effects::ShaderLibDef;

	class Renderer;
	class ShaderVariantManager;

	class ShaderLibrary
	{
	public:
		MAKE_NO_COPY(ShaderLibrary)

		ShaderLibrary();

		ShaderLibrary(Renderer& renderer, const ShaderLibDef& def);

		ShaderLibrary(Renderer& renderer, ShaderLibDef&& def);

		~ShaderLibrary();

		/// <summary>
		/// Retrieves interface for querying string IDs used in library resources
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns a new material using the given effect
		/// </summary>
		Material GetMaterial(uint effectNameID);

		/// <summary>
		/// Returns a new Compute shader instance using the given shader name
		/// </summary>
		ComputeInstance GetComputeInstance(uint nameID);

		/// <summary>
		/// Returns a new material using the given effect
		/// </summary>
		Material GetMaterial(string_view effectName);

		/// <summary>
		/// Returns a new Compute shader instance using the given shader name
		/// </summary>
		ComputeInstance GetComputeInstance(string_view name);

	private:
		std::unique_ptr<ShaderVariantManager> pManager;

	};
}
