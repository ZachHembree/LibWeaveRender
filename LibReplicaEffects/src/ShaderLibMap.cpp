#include "pch.hpp"
#include "ReplicaEffects/ShaderLibMap.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"

using namespace Replica;
using namespace Replica::Effects;

ShaderLibMap::ShaderLibMap() = default;

ShaderLibMap::~ShaderLibMap() = default;

const StringIDMap& ShaderLibMap::GetStringMap() const { return pRegMap->GetStringMap(); }

ShaderLibMap::ShaderLibMap(ShaderLibDef&& def) :
	platform(std::move(def.platform)),
	repoMaps(def.repos.GetLength()),
	pRegMap(new ShaderRegistryMap(std::move(def.regData)))
{ }

/// <summary>
/// Returns the shader by shaderID and variantID
/// </summary>
ShaderDefHandle ShaderLibMap::GetShader(uint shaderID) const
{
	REP_ASSERT_MSG(shaderID != -1, "Shader ID invalid");
	return ShaderDefHandle(*pRegMap, shaderID);
}

/// <summary>
/// Returns the effect with the given effectID and variantID
/// </summary>
EffectDefHandle ShaderLibMap::GetEffect(uint effectID) const
{
	REP_ASSERT_MSG(effectID != -1, "Effect ID invalid");
	return EffectDefHandle(*pRegMap, effectID);
}

const VariantRepoMap& ShaderLibMap::GetVariantMap(int mapID) const { return repoMaps[mapID]; }

uint ShaderLibMap::GetVariantMapCount() const { return (uint)repoMaps.GetLength(); }
