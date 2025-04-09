#pragma once
#include <concepts>
#include "ReplicaUtils/Serialization.hpp"
#include "ReplicaEffects/ShaderData.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Deserializes a byte array into a ShaderLibDef
	/// </summary>
	ShaderLibDef GetDeserializedLibDef(string_view libData);

	/// <summary>
	/// Deserializes a byte array into a ShaderLibDef
	/// </summary>
	template <std::size_t N>
	constexpr ShaderLibDef GetDeserializedLibDef(const byte(&arr)[N]) noexcept
	{
		return GetDeserializedLibDef(string_view(reinterpret_cast<const char*>(&arr[0]), N));
	}

	template <class Archive>
	void serialize(Archive& ar, ConstDef& def)
	{
		ar(def.stringID, def.offset, def.size);
	}

	template <class Archive>
	void serialize(Archive& ar, ConstBufDef& def)
	{
		ar(def.stringID, def.size, def.layoutID);
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
	void serialize(Archive& ar, ShaderDef& def)
	{
		ar(
			def.fileStringID, def.byteCodeID, def.nameID, def.stage,
			def.threadGroupSize.x, def.threadGroupSize.y, def.threadGroupSize.z,
			def.inLayoutID, def.outLayoutID, def.resLayoutID, def.cbufGroupID
		);
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
			def.constants, def.cbufLayouts, def.cbufDefs, 
			def.ioElements, def.resources,
			def.cbufGroups, def.ioSignatures, def.resGroups,
			def.effectPasses, def.binaries, 
			def.shaders, def.effects
		);
	}

	template <class Archive>
	void serialize(Archive& ar, PlatformDef& def)
	{
		ar(def.compilerVersion, def.featureLevel, def.target);
	}

	template <class Archive>
	void serialize(Archive& ar, VariantRepoDef& def)
	{
		ar(def.src, def.flagIDs, def.modeIDs, def.variants);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderLibDef& def)
	{
		ar(def.platform, def.repos, def.regData);
	}

	template <class Archive>
	void serialize(Archive& ar, VariantRepoSrc& def)
	{
		ar(def.name, def.path);
	}

	template <class Archive>
	void serialize(Archive& ar, ShaderLibSrc& def)
	{
		ar(def.srcFiles);
	}
}