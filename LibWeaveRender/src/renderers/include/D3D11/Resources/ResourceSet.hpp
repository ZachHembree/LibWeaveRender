#pragma once
#include "WeaveUtils/Span.hpp"
#include "ResourceBase.hpp"
#include "ResourceHandles.hpp"
#include <unordered_map>

namespace Weave::D3D11
{
	class Sampler;
	class ConstantGroupMap;

	struct ConstantGroupBuffer
	{
		UniqueVector<byte> data;
		// Const data layout
		UniqueVector<ConstantDesc> constants;
		// stringID -> const Index
		std::unordered_map<uint, uint> stringConstMap;

		const ConstantDesc& GetOrAddValue(uint stringID, uint size);

		void SetValue(uint stringID, const Span<byte>& newValue);

		void Clear();
	};

	/// <summary>
	/// A reusable variable-length map for associating arbitrary resources with an Effect or Shader
	/// </summary>
	class ResourceSet
	{
	public:		
		using SamplerList = IDynamicArray<ResNamePair<const Sampler>>;
		using SRVList = IDynamicArray<ResNamePair<const IShaderResource>>;
		using UAVList = IDynamicArray<ResNamePair<IUnorderedAccess>>;
		using ConstantData = IDynamicArray<Span<byte>>;

		MAKE_NO_COPY(ResourceSet)

		ResourceSet();

		ResourceSet(ResourceSet&&) noexcept;

		ResourceSet& operator=(ResourceSet&&) noexcept;

		ResourceHandle<const Sampler, const Sampler> GetSampler(uint stringID) 
		{ 
			return ResourceHandle<const Sampler, const Sampler>(stringID, sampMap); 
		}

		template<typename HandleT>
		HandleT GetSRV(uint stringID) { return HandleT(stringID, srvMap); }

		template<typename HandleT>
		HandleT GetUAV(uint stringID) { return HandleT(stringID, uavMap); }

		template<typename T>
		ConstantHandle<T> GetConstant(uint stringID) { return ConstantHandle<T>(*this, stringID); }

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
		friend ConstantDescHandle;

		ConstantGroupBuffer constants;
		ResViewBuffer<const Sampler> sampMap;
		ResViewBuffer<const IShaderResource> srvMap;
		ResViewBuffer<IUnorderedAccess> uavMap;
	};
}