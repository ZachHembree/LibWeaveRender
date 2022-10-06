#pragma once
#include "ReplicaUtils.hpp"
#include <unordered_map>

namespace Replica::D3D11
{
	template<typename T>
	class ResourceMap
	{
	public:
		ResourceMap() = default;
		ResourceMap(const ResourceMap&) = default;
		ResourceMap(ResourceMap&&) = default;
		ResourceMap& operator=(const ResourceMap& other) = default;
		ResourceMap& operator=(ResourceMap&& other) = default;

		ResourceMap(const IDynamicArray<string_view>& def) :
			resources(def.GetLength())
		{
			for (uint i = 0; i < def.GetLength(); i++)
			{
				resourceMap.emplace(def[i], i);
			}
		}

		ResourceMap(const std::initializer_list<string_view>& list) :
			ResourceMap(UniqueArray(list))
		{ }

		void SetResource(string_view name, T* pRes)
		{
			auto pair = resourceMap.find(name);

			if (pair != resourceMap.end())
			{
				uint i = pair->second;
				resources[i] = pRes;
			}
		}

		void Validate() 
		{ 
			for (int i = 0; i < resources.GetLength(); i++) 
				GFX_ASSERT(resources[i] != nullptr, "Cannot get null resources.");
		}

		void SetResource(string_view name, ComPtr<T>& pRes) { SetResource(name, pRes.Get()); }

		const IDynamicArray<T*>& GetResources() { return resources; }

		size_t GetCount() { return resources.GetLength(); }

	private:
		DynamicArray<T*> resources;
		std::unordered_map<string_view, uint> resourceMap;

	};
}