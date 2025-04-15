#pragma once
#include <concepts>
#include "WeaveUtils/Serialization.hpp"
#include "WeaveEffects/ShaderData.hpp"

namespace Weave::Effects
{
	template <class Archive>
	inline void serialize(Archive& ar, ConstDef& def)
	{
		ar(def.stringID, def.offset, def.size);
	}

	template <class Archive>
	inline void serialize(Archive& ar, ConstBufDef& def)
	{
		ar(def.stringID, def.size, def.layoutID);
	}

	template <class Archive>
	inline void serialize(Archive& ar, IOElementDef& def)
	{
		ar(def.semanticID, def.semanticIndex, def.dataType, def.componentCount, def.size);
	}

	template <class Archive>
	inline void serialize(Archive& ar, ResourceDef& def)
	{
		ar(def.stringID, def.type, def.slot);
	}

	template <class Archive>
	inline void serialize(Archive& ar, ShaderDef& def)
	{
		ar(
			def.fileStringID, def.byteCodeID, def.nameID, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayoutID, def.outLayoutID, def.resLayoutID, def.cbufGroupID
		);
	}

	template <class Archive>
	inline void serialize(Archive& ar, EffectDef& def)
	{
		ar(def.nameID, def.passGroupID);
	}

	template <class Archive>
	inline void serialize(Archive& ar, ShaderVariantDef& def)
	{
		ar(def.shaderID, def.variantID);
	}

	template <class Archive>
	inline void serialize(Archive& ar, EffectVariantDef& def)
	{
		ar(def.effectID, def.variantID);
	}

	template <class Archive>
	inline void serialize(Archive& ar, VariantDef& def)
	{
		ar(def.effects, def.shaders);
	}

	template<class Archive, typename T>
	inline void serialize(Archive& ar, SpanVector<T>& vec)
	{
		ar(vec.spans, vec.data);
	}

	template <class Archive>
	inline void serialize(Archive& ar, PlatformDef& def)
	{
		ar(def.compilerVersion, def.featureLevel, def.target);
	}

	template <class Archive>
	inline void serialize(Archive& ar, VariantRepoDef& def)
	{
		ar(def.src, def.flagIDs, def.modeIDs, def.variants);
	}

	template <class Archive>
	inline void serialize(Archive& ar, VariantRepoSrc& def)
	{
		ar(def.name, def.path);
	}

	template <class Archive>
	inline void serialize(Archive& ar, ShaderLibSrc& def)
	{
		ar(def.srcFiles);
	}

	template <class Archive>
	inline void save(Archive& ar, const ShaderLibDef::Handle& def)
	{
		ar(*def.pPlatform, *def.pRepos, def.regHandle, def.strMapHandle);
	}

	template <class Archive>
	inline void save(Archive& ar, const ShaderRegistryDef::Handle& def)
	{
		ar(
			*def.pConstants, *def.pCBufDefs, *def.pIOElements,
			*def.pResources, *def.pShaders, *def.pEffects,
			*def.pIDGroups, *def.pBinSpans
		);
	}

	template <class Archive>
	inline void load(Archive& ar, ShaderLibDef& def)
	{
		ar(def.platform, def.repos, def.regData, def.stringIDs);
	}

	template <class Archive>
	inline void load(Archive& ar, ShaderRegistryDef& def)
	{
		ar(
			def.constants, def.cbufDefs, def.ioElements,
			def.resources, def.shaders, def.effects,
			def.idGroups, def.binSpans
		);
	}
}