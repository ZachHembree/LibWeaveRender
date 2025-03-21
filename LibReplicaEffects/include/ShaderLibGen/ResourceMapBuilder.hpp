#pragma once
#include "ShaderLibGen/ShaderData.hpp"
#include <StringIDBuilder.hpp>
#include <unordered_set>
#include <functional>

namespace Replica::Effects
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

		CBufGroup = 5,
		IOLayout = 6,
		ResGroup = 7,

		Shader = 8,
		Effect = 9
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
		int GetResourceCount() const { return resCount; }

		/// <summary>
		/// Returns the total number of unique resources of all types, not counting strings
		/// </summary>
		int GetUniqueResCount() const { return uniqueResCount; }

		/* GetOrAdd(Member) functions return an integer ID that uniquely corresponds to the given definition.
		* If a definition has not been seen before, it will be added and a new ID will be generated. 
		* 
		* StringIDs are separate from shader definition types. */

		uint GetOrAddStringID(string&& str);

		uint GetOrAddStringID(string_view str);

		uint GetOrAddConstant(const ConstDef& constDef);

		uint GetOrAddConstantBuffer(ConstBufDef&& cbufDef);

		uint GetOrAddIOElement(const IOElementDef& ioDef);

		uint GetOrAddResource(const ResourceDef& resDef);

		uint GetOrAddCBufGroup(DynamicArray<uint>&& bufGroup);

		uint GetOrAddIOLayout(DynamicArray<uint>&& signature);

		uint GetOrAddResGroup(DynamicArray<uint>&& resGroup);

		uint GetOrAddShader(ShaderDef&& shader);

		uint GetOrAddEffect(EffectDef&& effect);

		/* Get(Member) returns an immutable reference to the object corresponding to the given ID. */

		const ConstDef& GetConstant(const uint id) const;

		const ConstBufDef& GetConstantBuffer(const uint id) const;

		const IOElementDef& GetIOElement(const uint id) const;

		const ResourceDef& GetResource(const uint id) const;

		const IDynamicArray<uint>& GetCBufGroup(const uint id) const;

		const IDynamicArray<uint>& GetIOLayout(const uint id) const;

		const IDynamicArray<uint>& GetResGroup(const uint id) const;

		const ShaderDef& GetShader(const uint id) const;

		const EffectDef& GetEffect(const uint id) const;

		ShaderRegistryDef ExportDefinition() const;

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

		StringIDBuilder stringIDs;
		std::unordered_set<HashHandle> resourceSet;

		HashableVector<ConstDef, ResourceType::Constant> constants;
		HashableVector<ConstBufDef, ResourceType::ConstantBuffer> cbufDefs;
		HashableVector<IOElementDef, ResourceType::IOElement> ioElements;
		HashableVector<ResourceDef, ResourceType::Resource> resources;

		HashableVector<DynamicArray<uint>, ResourceType::CBufGroup> cbufGroups;
		HashableVector<DynamicArray<uint>, ResourceType::IOLayout> ioSignatures;
		HashableVector<DynamicArray<uint>, ResourceType::ResGroup> resGroups;

		HashableVector<ShaderDef, ResourceType::Shader> shaders;
		HashableVector<EffectDef, ResourceType::Effect> effects;

		int resCount;
		int uniqueResCount;

		template<typename T, ResourceType type>
		uint GetOrAddValue(T&& newValue, HashableVector<T, type>& values)
		{
			const uint index = (uint)values.GetLength();
			const uint newID = SetResourceType(index, type);

			// Temporarily store new value in vector to ensure valid handle
			values.emplace_back(std::move(newValue));
			// Create handle and search set
			HashHandle handle(newID, values.GetHashFunc(), values.GetEqFunc());
			const auto& it = resourceSet.find(handle);

			resCount++;

			if (it != resourceSet.end())
			{
				// Value already existed, remove from vector and return existing ID
				values.pop_back();
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