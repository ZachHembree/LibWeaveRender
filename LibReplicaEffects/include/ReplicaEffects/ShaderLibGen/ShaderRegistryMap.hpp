#pragma once
#include "ReplicaEffects/ShaderData.hpp"

namespace Replica::Effects
{
	class VariantDefHandle;

	/// <summary>
	/// Read-only interface for deduplicated shader resources
	/// </summary>
	class ShaderRegistryMap
	{
	public:
		MAKE_NO_COPY(ShaderRegistryMap)

		ShaderRegistryMap(ShaderRegistryDef&& def);

		ShaderRegistryMap(const ShaderRegistryDef& def);

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const StringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns true if the string exists in the map and retrieves its ID
		/// </summary>
		bool TryGetStringID(string_view str, uint& id) const;

		/// <summary>
		/// Returns the string_view corresponding to the given ID
		/// </summary>
		string_view GetString(uint id) const;

		/// <summary>
		/// Returns the number of unique strings in the map
		/// </summary>
		uint GetStringCount() const;

		const EffectDef& GetEffect(uint effectID) const;

		const ShaderDef& GetShader(uint shaderID) const;

		const IDynamicArray<uint>& GetResourceGroup(uint groupID) const;

		const IDynamicArray<uint>& GetIOLayout(uint layoutID) const;

		const IDynamicArray<uint>& GetCBufGroup(uint groupID) const;

		const ResourceDef& GetResource(const uint resID) const;

		const IOElementDef& GetIOElement(const uint id) const;

		const ConstBufDef& GetConstBuf(const uint cbufID) const;

		const ConstDef& GetConstant(const uint constID) const;

	private:
		StringIDMap stringMap;
		
		UniqueArray<ConstDef> constants;
		UniqueArray<ConstBufDef> cbufDefs;
		UniqueArray<IOElementDef> ioElements;
		UniqueArray<ResourceDef> resources;

		UniqueArray<DynamicArray<uint>> cbufGroups;
		UniqueArray<DynamicArray<uint>> ioSignatures;
		UniqueArray<DynamicArray<uint>> resGroups;

		UniqueArray<ShaderDef> shaders;
		UniqueArray<EffectDef> effects;
	};
}