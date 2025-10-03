#pragma once
#include "WeaveEffects/ShaderData.hpp"

namespace Weave::Effects
{
	class VariantDefHandle;

	/// <summary>
	/// Read-only interface for deduplicated shader resources
	/// </summary>
	class ShaderRegistryMap
	{
	public:
		MAKE_NO_COPY(ShaderRegistryMap)

		ShaderRegistryMap(const StringIDMapDef::Handle& strDef, const ShaderRegistryDef::Handle& def);

		ShaderRegistryMap(StringIDMapDef&& strDef, ShaderRegistryDef&& def);

		/// <summary>
		/// Returns string ID lookup map
		/// </summary>
		const IStringIDMap& GetStringMap() const;

		/// <summary>
		/// Returns true if the string exists in the map and retrieves its ID
		/// </summary>
		bool TryGetStringID(string_view str, uint& id) const;

		/// <summary>
		/// Returns the string_view corresponding to the given ID
		/// </summary>
		StringSpan GetString(uint id) const;

		/// <summary>
		/// Returns the number of unique strings in the map
		/// </summary>
		uint GetStringCount() const;

		const EffectDef& GetEffect(uint effectID) const;

		const ShaderDef& GetShader(uint shaderID) const;

		ByteSpan GetByteCode(uint byteCodeID) const;

		IDSpan GetIDGroup(uint groupID) const;

		const ResourceDef& GetResource(const uint resID) const;

		const IOElementDef& GetIOElement(const uint id) const;

		const ConstBufDef& GetConstBuf(const uint cbufID) const;

		const ConstDef& GetConstant(const uint constID) const;

		ShaderRegistryDef::Handle GetDefinition() const;

	private:
		std::unique_ptr<ShaderRegistryDef> pRegDef;
		std::unique_ptr<IStringIDMap> pStringIDs;

	};
}