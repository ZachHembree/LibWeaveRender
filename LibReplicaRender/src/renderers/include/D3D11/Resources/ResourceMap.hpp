#pragma once
#include <unordered_map>
#include "../InternalD3D11.hpp"
#include "ReplicaEffects/ShaderDataHandles.hpp"
#include "ResourceSet.hpp"

namespace Replica::D3D11
{
	using Effects::ResourceDef;
	using Effects::ShaderTypes;

	/// <summary>
	/// Template for mapping shader resources to pipeline slots using string IDs
	/// </summary>
	template<typename T>
	class ResourceMap
	{
	public:
		using ResView = ResourceSet::ResView<T>;
		using DataSrc = IDynamicArray<ResView>;

		ResourceMap() = default;
		ResourceMap(const ResourceMap&) = default;
		ResourceMap(ResourceMap&&) = default;
		ResourceMap& operator=(const ResourceMap& other) = default;
		ResourceMap& operator=(ResourceMap&& other) = default;

		ResourceMap(const ResourceGroupHandle& resources, ShaderTypes type)
		{
			uint count = 0;

			for (int i = 0; i < resources.GetLength(); i++)
			{
				if (resources[i].GetHasFlags(type))
				{
					resourceMap.emplace(resources[i].stringID, count);
					count++;
				}
			}
		}

		void GetResources(const DataSrc& src, IDynamicArray<T*>& dst) const
		{
			const size_t count = GetCount();
			GFX_ASSERT(dst.GetLength() >= count, "Resource dst array too small")
			memset(dst.GetPtr(), 0u, count * sizeof(T**));

			for (const ResView& view : src)
			{
				const auto& it = resourceMap.find(view.stringID);

				if (it != resourceMap.end())
				{
					const uint index = it->second;
					dst[index] = view.pView;
				}
			}
		}

		/// <summary>
		/// Returns the number of resources in the map
		/// </summary>
		size_t GetCount() const { return resourceMap.size(); }

	private:
		// StringID to index
		std::unordered_map<uint, uint> resourceMap;
	};

	class SamplerMap : public ResourceMap<ID3D11SamplerState>
	{
		using ResourceMap<ID3D11SamplerState>::ResourceMap;
	};

	class ResourceViewMap : public ResourceMap<ID3D11ShaderResourceView>
	{ 
		using ResourceMap<ID3D11ShaderResourceView>::ResourceMap;
	};

	class UnorderedAccessMap : public ResourceMap<ID3D11UnorderedAccessView>
	{
		using ResourceMap<ID3D11UnorderedAccessView>::ResourceMap;
	};
}