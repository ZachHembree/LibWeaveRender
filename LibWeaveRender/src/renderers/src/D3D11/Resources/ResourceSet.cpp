#include "pch.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Resources/ConstantGroupMap.hpp"

using namespace Weave;
using namespace Weave::D3D11;

ResourceSet::ResourceSet() = default;

ResourceSet::ResourceSet(ResourceSet&&) noexcept = default;

ResourceSet& ResourceSet::operator=(ResourceSet&&) noexcept = default;

void ResourceSet::SetSampler(uint stringID, const Sampler& samp) { sampMap.SetResource(stringID, &samp); }

void ResourceSet::SetSRV(uint stringID, const IShaderResource& srv) { srvMap.SetResource(stringID, &srv); }

void ResourceSet::SetUAV(uint stringID, IUnorderedAccess& uav) { uavMap.SetResource(stringID, &uav); }

void ResourceSet::SetConstant(uint stringID, const Span<byte>& newValue) { constants.SetValue(stringID, newValue); }

const ResourceSet::SamplerList& ResourceSet::GetSamplers() const { return sampMap.data; }

const ResourceSet::SRVList& ResourceSet::GetSRVs() const { return srvMap.data; }

const ResourceSet::UAVList& ResourceSet::GetUAVs() const { return uavMap.data; }

uint ResourceSet::GetConstantCount() const { return (uint)constants.constants.GetLength(); }

const ConstantDesc& ConstantGroupBuffer::GetOrAddValue(uint stringID, uint size)
{
	const auto& it = stringConstMap.find(stringID);

	if (it != stringConstMap.end()) // Update value
	{
		return constants[it->second];
	}
	else // Add new value
	{
		const uint index = (uint)constants.GetLength();
		const uint offset = (uint)data.GetLength();

		// Add constant descriptor
		constants.EmplaceBack(stringID, size, offset);
		// Map stringID -> constant desc index
		stringConstMap.emplace(stringID, index);
		// Allocate new constant
		data.Resize(offset + size);

		return constants.GetBack();
	}
}

void ConstantGroupBuffer::SetValue(uint stringID, const Span<byte>& newValue)
{
	const ConstantDesc& value = GetOrAddValue(stringID, (uint)newValue.GetLength());
	WV_ASSERT_MSG(newValue.GetLength() == value.size, "Constant size does not match existing value");
	memcpy(&data[value.offset], newValue.GetData(), (uint)newValue.GetLength());
}

void ConstantGroupBuffer::Clear()
{
	data.Clear();
	constants.Clear();
	stringConstMap.clear();
}

const ResourceSet::ConstantData& ResourceSet::GetMappedConstants(const ConstantGroupMap& map) const
{
	return map.GetData(constants);
}
