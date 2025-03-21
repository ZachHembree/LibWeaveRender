#pragma once
#include "ShaderData.hpp"

using namespace Replica;
using namespace Replica::Effects;

template<>
struct std::hash<ConstDef>
{
	size_t operator()(const ConstDef& def) const noexcept
	{
		return GetCombinedHash(def.stringID, def.offset, def.size);
	}
};

template<>
struct std::hash<ConstBufDef>
{
	size_t operator()(const ConstBufDef& def) const noexcept
	{
		return GetCombinedHash(def.stringID, def.members, def.size);
	}
};

template<>
struct std::hash<IOElementDef>
{
	size_t operator()(const IOElementDef& def) const noexcept
	{
		return GetCombinedHash(def.semanticID, def.semanticIndex, def.dataType, def.componentCount, def.size);
	}
};

template<>
struct std::hash<ResourceDef>
{
	size_t operator()(const ResourceDef& def) const noexcept
	{
		return GetCombinedHash(def.stringID, def.type, def.slot);
	}
};

template<>
struct std::hash<tvec3<uint>>
{
	size_t operator()(const tvec3<uint>& vec) const noexcept
	{
		return GetCombinedHash(vec.x, vec.y, vec.z);
	}
};

template<>
struct std::hash<ShaderDef>
{
	size_t operator()(const ShaderDef& def) const noexcept
	{
		return GetCombinedHash(def.fileStringID, def.binSrc, def.nameID, def.stage, def.threadGroupSize);
	}
};

template<>
struct std::hash<EffectPass>
{
	size_t operator()(const EffectPass& def) const noexcept
	{
		return GetCombinedHash(def.shaderIDs);
	}
};

template<>
struct std::hash<EffectDef>
{
	size_t operator()(const EffectDef& def) const noexcept
	{
		return GetCombinedHash(def.nameID, def.passes);
	}
};