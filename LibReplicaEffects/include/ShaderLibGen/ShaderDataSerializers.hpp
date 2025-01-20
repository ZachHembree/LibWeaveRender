#pragma once
#include <concepts>
#include "Serialization.hpp"
#include "ShaderLibGen/ShaderParser/ShaderData.hpp"

namespace Replica::Effects
{
	template <class Archive>
	void serialize(Archive& ar, ConstDef& def)
	{
		ar(def.name, def.offset, def.size);
	}

	template <class Archive>
	void serialize(Archive& ar, IOElementDef& def)
	{
		ar(def.semanticName, def.semanticIndex, def.size);
	}

	template <class Archive>
	void serialize(Archive& ar, ResourceDef& def)
	{
		ar(def.name, def.type, def.slot);
	}

	template <class Archive>
	requires (!cereal::traits::is_text_archive<Archive>::value)
	void serialize(Archive& ar, ShaderDef& def)
	{
		ar(
			def.fileName, def.binSrc, def.name, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayout, def.outLayout, def.res, def.constBufs, def.variantID
		);
	}

	template <class Archive>
	requires cereal::traits::is_text_archive<Archive>::value
	void save(Archive& ar, const ShaderDef& def)
	{
		ar(
			def.fileName, def.name, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayout, def.outLayout, def.res, def.constBufs, def.variantID
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
			def.fileName, def.name, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayout, def.outLayout, def.res, def.constBufs, def.variantID
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
		ar(def.name, def.passes, def.variantID);
	}

	template <class Archive>
	void serialize(Archive& ar, VariantDef& def)
	{
		ar(def.effects, def.shaders);
	}

	template <class Archive>
	void serialize(Archive& ar, PlatformDef& def)
	{
		ar(def.compilerVersion, def.shaderModel, def.target);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderLibDef& def)
	{
		ar(def.platform, def.flagNames, def.modeNames, def.variants);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderRepoDef& def)
	{
		ar(def.libraries);
	}
}