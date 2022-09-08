#pragma once
#include "D3DUtils.hpp"
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

		ResourceMap(const IDynamicCollection<wstring_view>& def) :
			resources(def.GetLength())
		{
			for (uint i = 0; i < def.GetLength(); i++)
			{
				resourceMap.emplace(def[i], i);
			}
		}

		ResourceMap(const std::initializer_list<wstring_view>& list) :
			ResourceMap(UniqueArray(list))
		{ }

		void SetResource(wstring_view name, T* pRes)
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

		void SetResource(wstring_view name, ComPtr<T>& pRes) { SetResource(name, pRes.Get()); }

		const IDynamicCollection<T*>& GetResources() { Validate(); return resources; }

	private:
		DynamicArray<T*> resources;
		std::unordered_map<wstring_view, uint> resourceMap;

	};
}