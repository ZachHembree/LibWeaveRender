#pragma once
#include <unordered_set>
#include <functional>
#include "WeaveEffects/ShaderData.hpp"
#include "WeaveUtils/StringIDBuilder.hpp"
#include "WeaveUtils/VectorSpan.hpp"
#include "WeaveUtils/ObjectPool.hpp"

namespace Weave::Effects
{
	/// <summary>
	/// Shader resource types that can be deduplicated
	/// </summary>
	enum class ResourceType : byte
	{
		Undefined = 0,

		Constant = 1,
		ConstantBuffer = 2,
		IOElement = 3,
		Resource = 4,

		Shader = 5,
		Effect = 6,

		ByteCode = 7,
		IDGroups = 8,
	};

	/// <summary>
	/// A wrapper for hashing heterogeneous types
	/// </summary>
	class HashHandle
	{
	public:
		using HashFunc = std::function<size_t(const HashHandle&)>;
		using IsEqFunc = std::function<bool(const HashHandle&, const HashHandle&)>;

		HashHandle();

		HashHandle(const uint id, const HashFunc& getHash, const IsEqFunc& getIsEq);

		uint GetID() const;

		uint GetIndex() const;

		ResourceType GetType() const;

		size_t GetHash() const;

		bool operator==(const HashHandle& rhs) const;

	private:
		const HashFunc* pGetHash;
		const IsEqFunc* pGetIsEqual;
		uint id;
	};
}

namespace std
{
	using HashHandle = Weave::Effects::HashHandle;

	/// <summary>
	/// HashHandle std::hash specialization for unordered_set
	/// </summary>
	template<>
	struct hash<HashHandle>
	{
		size_t operator()(const HashHandle& handle) const noexcept { return handle.GetHash(); }
	};
}

namespace Weave::Effects
{
	class ShaderRegistryMap;
	class ShaderDefHandle;
	class EffectDefHandle;

	/// <summary>
	/// Builds a set of unique shaders, effects and supporting resources and assigns each a unique uint32_t ID
	/// </summary>
	class ShaderRegistryBuilder
	{
	public:
		using HashFunc = HashHandle::HashFunc;
		using IsEqFunc = HashHandle::IsEqFunc;

		MAKE_IMMOVABLE(ShaderRegistryBuilder)

		ShaderRegistryBuilder();

		/// <summary>
		/// Clears internal storage and resets map to initial state
		/// </summary>
		void Clear();

		/// <summary>
		/// Returns the total number of resources, not counting strings
		/// </summary>
		uint GetResourceCount() const;

		/// <summary>
		/// Returns the total number of unique resources of all types, not counting strings
		/// </summary>
		uint GetUniqueResCount() const;

		/// <summary>
		/// Returns the total number of unique shaders
		/// </summary>
		uint GetShaderCount() const;

		/// <summary>
		/// Returns the total number of unique effects
		/// </summary>
		uint GetEffectCount() const;

		/* GetOrAdd(Member) functions return an integer ID that uniquely corresponds to the given definition.
		* If a definition has not been seen before, it will be added and a new ID will be generated.
		*
		* StringIDs are separate from shader definition types. */

		uint GetOrAddStringID(string_view str);

		uint GetOrAddConstant(const ConstDef& constDef);

		uint GetOrAddConstantBuffer(const ConstBufDef& cbufDef);

		uint GetOrAddIOElement(const IOElementDef& ioDef);

		uint GetOrAddResource(const ResourceDef& resDef);

		uint GetOrAddShader(const ShaderDef& shader);

		uint GetOrAddEffect(const EffectDef& effect);

		uint GetOrAddIDGroup(const IDynamicArray<uint>& idGroup);

		uint GetOrAddShaderBin(const IDynamicArray<byte>& byteCode);

		/* ShaderRegistryMap Copy Utils
		* Return IDs to equivalent definitions in this registry builder. If the given definition has no equivalent, it will be copied 
		* into the builder's registry, with new resource and string IDs as needed. Used for copying definitions from a finalized 
		* ShaderRegistryMap.
		*/

		uint GetOrAddShader(const ShaderDefHandle& shaderDef);

		uint GetOrAddEffect(const EffectDefHandle& effectDef);

		/* Get(Member) returns an immutable reference to the object corresponding to the given ID. */

		const ConstDef& GetConstant(const uint id) const;

		const ConstBufDef& GetConstantBuffer(const uint id) const;

		const IOElementDef& GetIOElement(const uint id) const;

		const ResourceDef& GetResource(const uint id) const;

		const ShaderDef& GetShader(const uint id) const;

		const EffectDef& GetEffect(const uint id) const;

		IDSpan GetIDGroup(const uint id) const;

		ByteSpan GetShaderBin(const uint id) const;

		/// <summary>
		/// Returns a temporary buffer for building a collection of unique IDs. Must be returned
		/// before clearing or exporting the registry.
		/// </summary>
		Vector<uint> GetTmpIDBuffer();

		/// <summary>
		/// Returns an ID buffer to the pool for reuse.
		/// </summary>
		void ReturnTmpIDBuffer(Vector<uint>&& buf);

		/// <summary>
		/// Returns a raw byte vector for temporarily storing shader bytecode. Must be returned
		/// before clearing or exporting the registry.
		/// </summary>
		Vector<byte> GetTmpByteBuffer();

		/// <summary>
		/// Returns the bytecode vector to the pool for reuse.
		/// </summary>
		void ReturnTmpByteBuffer(Vector<byte>&& buf);

		/// <summary>
		/// Returns reference to string ID map
		/// </summary>
		const StringIDBuilder& GetStringIDBuilder() const;

		/// <summary>
		/// Returns serializable handles to registry definitions
		/// </summary>
		ShaderRegistryDef::Handle GetDefinition() const;

		/// <summary>
		/// Returns index and resource enum combined into an ID
		/// </summary>
		static uint SetResourceType(uint index, ResourceType type);

		/// <summary>
		/// Parses resource type from ID
		/// </summary>
		static ResourceType GetResourceType(uint id);

		/// <summary>
		/// Returns index from ID
		/// </summary>
		static uint GetIndex(uint id);

	private:
		/// <summary>
		/// Vector template that allows hashing members with HashHandles
		/// </summary>
		template<typename VecT, ResourceType typeEnum>
		class HashableVector : public VecT
		{
		public:
			using value_type = typename VecT::value_type;
			using const_reference = typename VecT::const_reference;
			static constexpr ResourceType ResType = typeEnum;

			HashableVector()
			{
				GetIsMemberEq = [this](const HashHandle& lhs, const HashHandle& rhs) -> bool
				{
					return this->at(lhs.GetIndex()) == this->at(rhs.GetIndex());
				};
				GetMemberHash = [this](const HashHandle& handle) -> size_t
				{
					return std::hash<value_type>{}(this->at(handle.GetIndex()));
				};
			}

			const HashFunc& GetHashFunc() const { return GetMemberHash; }

			const IsEqFunc& GetEqFunc() const { return GetIsMemberEq; }

			const_reference GetValue(const uint id) const
			{
				if (GetResourceType(id) == typeEnum)
					return this->at(GetIndex(id));
				else
					FX_THROW("Resource type ID mismatch");
			}

		private:
			HashFunc GetMemberHash;
			IsEqFunc GetIsMemberEq;
		};

		ObjectPool<Vector<uint>> idBufPool;
		ObjectPool<Vector<byte>> byteCodePool;

		StringIDBuilder stringIDs;
		std::unordered_set<HashHandle> resourceSet;

		UniqueVector<uint> idBuffer;
		UniqueVector<byte> byteBuffer;

		HashableVector<UniqueVector<ConstDef>, ResourceType::Constant> constants;
		HashableVector<UniqueVector<ConstBufDef>, ResourceType::ConstantBuffer> cbufDefs;
		HashableVector<UniqueVector<IOElementDef>, ResourceType::IOElement> ioElements;
		HashableVector<UniqueVector<ResourceDef>, ResourceType::Resource> resources;

		HashableVector<UniqueVector<ShaderDef>, ResourceType::Shader> shaders;
		HashableVector<UniqueVector<EffectDef>, ResourceType::Effect> effects;

		HashableVector<SpanVector<uint>, ResourceType::IDGroups> idGroups;
		HashableVector<SpanVector<byte>, ResourceType::ByteCode> binSpans;

		uint resCount;
		uint uniqueResCount;

		// Temporary conversion cache for copying from a ShaderRegistryMap
		const ShaderRegistryMap* pCopySrc;
		std::unordered_map<ShaderDef, uint> shaderCopyCache;
		std::unordered_map<EffectDef, uint> effectCopyCache;

		template<typename T, typename VecT, ResourceType type>
		uint GetOrAddValue(const T& newValue, HashableVector<VecT, type>& values)
		{
			const uint index = (uint)values.GetLength();
			const uint newID = SetResourceType(index, type);

			// Temporarily store new value in vector to ensure valid handle
			values.Add(newValue);
			// Create handle and search set
			HashHandle handle(newID, values.GetHashFunc(), values.GetEqFunc());
			const auto& it = resourceSet.find(handle);
			resCount++;

			if (it != resourceSet.end())
			{
				// Value already existed, remove from vector and return existing ID
				values.RemoveBack();
				return it->GetID();
			}
			else
			{
				uniqueResCount++;
				resourceSet.emplace(handle);
				return handle.GetID();
			}
		}
	};
}