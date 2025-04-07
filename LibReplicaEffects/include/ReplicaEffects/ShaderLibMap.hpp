#pragma once
#include "VariantRepoMap.hpp"

namespace Replica::Effects
{
	class ShaderLibMap
	{
	public:
		MAKE_NO_COPY(ShaderLibMap)

		ShaderLibMap();

		ShaderLibMap(ShaderLibDef&& def);

		~ShaderLibMap();

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns the shader by shaderID and variantID
		/// </summary>
		ShaderDefHandle GetShader(uint shaderID) const;

		/// <summary>
		/// Returns the effect with the given effectID and variantID
		/// </summary>
		EffectDefHandle GetEffect(uint effectID) const;

		const VariantRepoMap& GetVariantMap(int mapID) const;

		uint GetVariantMapCount() const;

	private:
		/// <summary>
		/// Describes the platform targeted during compilation
		/// </summary>
		PlatformDef platform;

		/// <summary>
		/// Pointer to unique shader data map
		/// </summary>
		std::unique_ptr<ShaderRegistryMap> pRegMap;

		UniqueArray<VariantRepoMap> repoMaps;
	};
}