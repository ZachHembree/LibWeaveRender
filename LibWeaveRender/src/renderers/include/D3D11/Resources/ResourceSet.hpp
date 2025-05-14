#pragma once
#include "WeaveUtils/Span.hpp"
#include "ResourceBase.hpp"
#include "Sampler.hpp"
#include <unordered_map>

namespace Weave::D3D11
{
	class ConstantGroupMap;

	/// <summary>
	/// A reusable variable-length map for associating arbitrary resources with an Effect or Shader
	/// </summary>
	class ResourceSet
	{
	public:		
		MAKE_NO_COPY(ResourceSet)

		/// <summary>
		/// Stores a pointer to a resource view paired with a stringID
		/// </summary>
		template<typename T>
		struct ResView
		{
			uint stringID;
			T* pView;
		};

		using SamplerList = IDynamicArray<ResView<const Sampler>>;
		using SRVList = IDynamicArray<ResView<const IShaderResource>>;
		using UAVList = IDynamicArray<ResView<IUnorderedAccess>>;
		using ConstantData = IDynamicArray<Span<byte>>;

		/// <summary>
		/// Stores mappings for string ID <-> view lookup
		/// </summary>
		template<typename T>
		struct ViewMap
		{
			UniqueVector<ResView<T>> data;
			// stringID -> index
			std::unordered_map<uint, uint> stringViewMap;

			/// <summary>
			/// Updates or adds the resource with the given string ID
			/// </summary>
			void SetResource(uint stringID, T* pValue)
			{
				const auto& it = stringViewMap.find(stringID);

				if (it != stringViewMap.end())
				{
					data[it->second].pView = pValue;
				}
				else
				{
					stringViewMap.emplace(stringID, (uint)data.GetLength());
					data.EmplaceBack(stringID, pValue);
				}
			}

			/// <summary>
			/// Resets the collection
			/// </summary>
			void Clear()
			{
				data.Clear();
				stringViewMap.clear();
			}
		};

		struct Constant
		{
			uint stringID;
			uint size;
			// Position in byte vector
			uint offset;
		};

		struct ConstantGroup
		{
			UniqueVector<byte> data;
			// Const data layout
			UniqueVector<Constant> constants;
			// stringID -> const Index
			std::unordered_map<uint, uint> stringConstMap;

			void SetValue(uint stringID, const Span<byte>& newValue);

			void Clear();
		};

		ResourceSet();

		ResourceSet(ResourceSet&&) noexcept;

		ResourceSet& operator=(ResourceSet&&) noexcept;

		void SetSampler(uint stringID, const Sampler& samp);

		void SetSRV(uint stringID, const IShaderResource& srv);

		void SetUAV(uint stringID, IUnorderedAccess& uav);

		void SetConstant(uint stringID, const Span<byte>& newValue);

		template<typename T>
		void SetConstant(uint stringID, const T& value) { constants.SetValue(stringID, {(byte*)(&value), sizeof(T)}); }

		const SamplerList& GetSamplers() const;

		const ResourceSet::SRVList& GetSRVs() const;

		const ResourceSet::UAVList& GetUAVs() const;

		uint GetConstantCount() const;

		const ConstantData& GetMappedConstants(const ConstantGroupMap& map) const;

		void Clear()
		{
			constants.Clear();
			sampMap.Clear();
			srvMap.Clear();
			uavMap.Clear();
		}

	private:	
		ConstantGroup constants;
		ViewMap<const Sampler> sampMap;
		ViewMap<const IShaderResource> srvMap;
		ViewMap<IUnorderedAccess> uavMap;
	};
}