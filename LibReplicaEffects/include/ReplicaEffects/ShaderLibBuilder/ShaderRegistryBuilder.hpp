#pragma once
#include <unordered_set>
#include <functional>
#include "ReplicaEffects/ShaderData.hpp"
#include "ReplicaUtils/StringIDBuilder.hpp"
#include "ReplicaUtils/VectorSpan.hpp"

namespace Replica::Effects
{
	/// <summary>
	/// Shader resource types that can be deduplicated
	/// </summary>
	enum class ResourceType : byte
	{
		Undefined = 0,

		Constant = 1,
		ConstLayout = 2,
		ConstantBuffer = 3,
		IOElement = 4,
		Resource = 5,

		CBufGroup = 6,
		IOLayout = 7,
		ResGroup = 8,

		ByteCode = 9,
		Shader = 10,
		EffectPass = 11,
		Effect = 12,
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
	using HashHandle = Replica::Effects::HashHandle;

	/// <summary>
	/// HashHandle std::hash specialization for unordered_set
	/// </summary>
	template<>
	struct hash<HashHandle>
	{
		size_t operator()(const HashHandle& handle) const noexcept { return handle.GetHash(); }
	};
}

namespace Replica::Effects
{
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
		int GetResourceCount() const;

		/// <summary>
		/// Returns the total number of unique resources of all types, not counting strings
		/// </summary>
		int GetUniqueResCount() const;

		/* GetOrAdd(Member) functions return an integer ID that uniquely corresponds to the given definition.
		* If a definition has not been seen before, it will be added and a new ID will be generated. 
		* 
		* StringIDs are separate from shader definition types. */

		uint GetOrAddStringID(string_view str);

		uint GetOrAddConstant(const ConstDef& constDef);

		uint GetOrAddConstantBuffer(const ConstBufDef& cbufDef);

		uint GetOrAddIOElement(const IOElementDef& ioDef);

		uint GetOrAddResource(const ResourceDef& resDef);

		uint GetOrAddConstantLayout(const IDynamicArray<uint>& layout);

		uint GetOrAddCBufGroup(const IDynamicArray<uint>& bufGroup);

		uint GetOrAddIOLayout(const IDynamicArray<uint>& signature);

		uint GetOrAddResGroup(const IDynamicArray<uint>& resGroup);

		uint GetOrAddEffectPass(const IDynamicArray<uint>& pass);

		uint GetOrAddShaderBin(const IDynamicArray<byte>& byteCode);

		uint GetOrAddShader(const ShaderDef& shader);

		uint GetOrAddEffect(const EffectDef& effect);

		/* Get(Member) returns an immutable reference to the object corresponding to the given ID. */

		const ConstDef& GetConstant(const uint id) const;

		const ConstBufDef& GetConstantBuffer(const uint id) const;

		const IOElementDef& GetIOElement(const uint id) const;

		const ResourceDef& GetResource(const uint id) const;

		const IDynamicArray<uint>& GetConstantLayout(const uint id) const;

		const IDynamicArray<uint>& GetCBufGroup(const uint id) const;

		const IDynamicArray<uint>& GetIOLayout(const uint id) const;

		const IDynamicArray<uint>& GetResGroup(const uint id) const;

		const IDynamicArray<uint>& GetEffectPass(const uint id) const;

		const IDynamicArray<byte>& GetShaderBin(const uint id) const;

		const ShaderDef& GetShader(const uint id) const;

		const EffectDef& GetEffect(const uint id) const;

		ShaderRegistryDef ExportDefinition() const;

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
		template<typename T>
		using RegSpan = VectorSpan<UniqueVector<T>, T>;

		/// <summary>
		/// Vector template that allows hashing members with HashHandles
		/// </summary>
		template<typename T, ResourceType typeEnum>
		class HashableVector : public UniqueVector<T>
		{
		public:			
			static constexpr ResourceType ResType = typeEnum;

			HashableVector()
			{
				GetIsMemberEq = [this](const HashHandle& lhs, const HashHandle& rhs) -> bool
				{
					return this->at(lhs.GetIndex()) == this->at(rhs.GetIndex());
				};
				GetMemberHash = [this](const HashHandle& handle) -> size_t
				{
					return std::hash<T>{}(this->at(handle.GetIndex()) );
				};
			}

			const HashFunc& GetHashFunc() const { return GetMemberHash; }

			const IsEqFunc& GetEqFunc() const { return GetIsMemberEq; }

			const T& GetValue(const uint id) const 
			{
				if (GetResourceType(id) == typeEnum)
					return this->at(GetIndex(id));
				else
					REP_THROW_MSG("Resource type ID mismatch");
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

		HashableVector<ConstDef, ResourceType::Constant> constants;
		HashableVector<ConstBufDef, ResourceType::ConstantBuffer> cbufDefs;
		HashableVector<IOElementDef, ResourceType::IOElement> ioElements;
		HashableVector<ResourceDef, ResourceType::Resource> resources;

		HashableVector<RegSpan<uint>, ResourceType::ConstLayout> cbufLayouts;
		HashableVector<RegSpan<uint>, ResourceType::CBufGroup> cbufGroups;
		HashableVector<RegSpan<uint>, ResourceType::IOLayout> ioSignatures;
		HashableVector<RegSpan<uint>, ResourceType::ResGroup> resGroups;
		HashableVector<RegSpan<uint>, ResourceType::EffectPass> effectPasses;

		HashableVector<RegSpan<byte>, ResourceType::ByteCode> binaries;
		HashableVector<ShaderDef, ResourceType::Shader> shaders;
		HashableVector<EffectDef, ResourceType::Effect> effects;

		int resCount;
		int uniqueResCount;

		template<typename T, ResourceType type>
		uint GetOrAddValue(const T& newValue, HashableVector<T, type>& values)
		{
			const uint index = (uint)values.GetLength();
			const uint newID = SetResourceType(index, type);

			// Temporarily store new value in vector to ensure valid handle
			values.EmplaceBack(newValue);
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
		
		template<typename T, ResourceType type>
		uint GetOrAddValue(const IDynamicArray<T>& ids, HashableVector<RegSpan<T>, type>& values, UniqueVector<T>& buffer)
		{
			const uint index = (uint)values.GetLength();
			const uint newID = SetResourceType(index, type);
			const uint spanStart = (uint)buffer.GetLength();

			// Temporarily store new value in vector to ensure valid handle
			buffer.AddRange(ids);
			const DynSpan<T>& layout = values.EmplaceBack(buffer, spanStart, ids.GetLength());
	
			// Create handle and search set
			HashHandle handle(newID, values.GetHashFunc(), values.GetEqFunc());
			const auto& it = resourceSet.find(handle);

			resCount++;

			if (it != resourceSet.end())
			{
				// Value already existed, remove from vector and return existing ID
				buffer.RemoveRange(spanStart, layout.GetLength());
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