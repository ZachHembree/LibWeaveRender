#pragma once
#include <concepts>
#include "ReplicaUtils/Serialization.hpp"
#include "ReplicaEffects/ShaderData.hpp"

namespace Replica::Effects
{
	template <class Archive>
	void serialize(Archive& ar, ConstDef& def)
	{
		ar(def.stringID, def.offset, def.size);
	}

	template <class Archive>
	void serialize(Archive& ar, ConstBufDef& def)
	{
		ar(def.stringID, def.size, def.members);
	}

	template <class Archive>
	void serialize(Archive& ar, IOElementDef& def)
	{
		ar(def.semanticID, def.semanticIndex, def.dataType, def.componentCount, def.size);
	}

	template <class Archive>
	void serialize(Archive& ar, ResourceDef& def)
	{
		ar(def.stringID, def.type, def.slot);
	}

	template <class Archive>
	requires (!cereal::traits::is_text_archive<Archive>::value)
	void serialize(Archive& ar, ShaderDef& def)
	{
		ar(
			def.fileStringID, def.binSrc, def.nameID, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayoutID, def.outLayoutID, def.resLayoutID, def.cbufGroupID
		);
	}

	template <class Archive>
	requires cereal::traits::is_text_archive<Archive>::value
	void save(Archive& ar, const ShaderDef& def)
	{
		ar(
			def.fileStringID, def.nameID, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayoutID, def.outLayoutID, def.resLayoutID, def.cbufGroupID
		);

		// Manual size tag
		ar(static_cast<cereal::size_type>(GetArrSize(def.binSrc)));
		// Write as base64 string
		ar.saveBinaryValue(def.binSrc.GetPtr(), GetArrSize(def.binSrc));
	}

	template <class Archive>
	requires cereal::traits::is_text_archive<Archive>::value
	void load(Archive& ar, ShaderDef& def)
	{
		ar(
			def.fileStringID, def.nameID, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayoutID, def.outLayoutID, def.resLayoutID, def.cbufGroupID
		);

		// Get array size
		cereal::size_type size = 0;
		ar(size);
		// Allocate and read
		def.binSrc = UniqueArray<byte>(size);
		ar.loadBinaryValue(def.binSrc.GetPtr(), size);
	}

	template <class Archive>
	void serialize(Archive& ar, EffectPass& def)
	{
		ar(def.shaderIDs);
	}

	template <class Archive>
	void serialize(Archive& ar, EffectDef& def)
	{
		ar(def.nameID, def.passes);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderVariantDef& def)
	{
		ar(def.shaderID, def.variantID);
	}

	template <class Archive>
	void serialize(Archive& ar, EffectVariantDef& def)
	{
		ar(def.effectID, def.variantID);
	}

	template <class Archive>
	void serialize(Archive& ar, VariantDef& def)
	{
		ar(def.effects, def.shaders);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderRegistryDef& def)
	{
		ar(
			def.stringIDs, 
			def.constants, def.cbufDefs, def.ioElements, def.resources,
			def.cbufGroups, def.ioSignatures, def.resGroups,
			def.shaders, def.effects
		);
	}

	template <class Archive>
	void serialize(Archive& ar, PlatformDef& def)
	{
		ar(def.compilerVersion, def.featureLevel, def.target);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderLibDef& def)
	{
		ar(def.platform, def.flagIDs, def.modeIDs, def.variants, def.regData);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderRepoDef& def)
	{
		ar(def.libraries);
	}
}