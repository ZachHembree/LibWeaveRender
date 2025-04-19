#pragma once
#include <unordered_map>
#include "../InternalD3D11.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "ResourceSet.hpp"

namespace Weave::D3D11
{
	using Effects::ResourceDef;
	using Effects::ShaderTypes;

	/// <summary>
	/// Template for mapping shader resources to pipeline slots using string IDs
	/// </summary>
	template<typename T, ShaderTypes TypeEnum>
	class ResourceMap
	{
	public:
		using ViewT = ResourceSet::ResView<T>;
		using DataT = IDynamicArray<ViewT>;
		static constexpr ShaderTypes Type = TypeEnum;

		ResourceMap() = default;
		ResourceMap(const ResourceMap&) = default;
		ResourceMap(ResourceMap&&) = default;
		ResourceMap& operator=(const ResourceMap& other) = default;
		ResourceMap& operator=(ResourceMap&& other) = default;

		ResourceMap(const std::optional<ResourceGroupHandle>& resources)
		{
			if (resources.has_value())
			{ 
				uint count = 0;

				for (int i = 0; i < resources->GetLength(); i++)
				{
					if ((*resources)[i].GetHasFlags(TypeEnum))
					{
						resourceMap.emplace((*resources)[i].stringID, count);
						count++;
					}
				}
			}
		}

		void GetResources(const DataT& src, IDynamicArray<T*>& dst) const
		{
			const size_t count = GetCount();
			D3D_ASSERT_MSG(dst.GetLength() >= count, "Resource dst array too small");
			memset(dst.GetData(), 0u, count * sizeof(T**));

			for (const ViewT& view : src)
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

	class SamplerMap : public ResourceMap<Sampler, ShaderTypes::Sampler>
	{
		using ResourceMap<Sampler, ShaderTypes::Sampler>::ResourceMap;
	};

	class ResourceViewMap : public ResourceMap<IShaderResource, ShaderTypes::Texture>
	{ 
		using ResourceMap<IShaderResource, ShaderTypes::Texture>::ResourceMap;
	};

	class UnorderedAccessMap : public ResourceMap<IUnorderedAccess, ShaderTypes::RandomWrite>
	{
		using ResourceMap<IUnorderedAccess, ShaderTypes::RandomWrite>::ResourceMap;
	};
}