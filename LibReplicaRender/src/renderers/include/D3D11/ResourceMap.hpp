#pragma once
#include "ReplicaUtils.hpp"
#include "ShaderLibGen/ShaderData.hpp"
#include <unordered_map>

namespace Replica::D3D11
{
	using Effects::ResourceDef;
	using Effects::ShaderTypes;

	/// <summary>
	/// Maps names to COM objects, but doesn't own them.
	/// </summary>
	template<typename T>
	class ResourceMap
	{
	public:
		ResourceMap() = default;
		ResourceMap(const ResourceMap&) = default;
		ResourceMap(ResourceMap&&) = default;
		ResourceMap& operator=(const ResourceMap& other) = default;
		ResourceMap& operator=(ResourceMap&& other) = default;

		ResourceMap(const IDynamicArray<ResourceDef>& resources, ShaderTypes type)
		{
			uint count = 0;

			for (const ResourceDef& res : resources)
			{
				if ( res.GetHasFlags(type) )
				{
					resourceMap.emplace(res.name, count);
					count++;
				}
			}

			this->resources = DynamicArray<T*>(count);
		}

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