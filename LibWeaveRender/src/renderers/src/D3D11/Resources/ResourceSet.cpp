#include "pch.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Resources/ConstantGroupMap.hpp"

using namespace Weave;
using namespace Weave::D3D11;

template<typename T>
using ResView = ResourceSet::ResView<T>;

ResourceSet::ResourceSet() = default;

ResourceSet::ResourceSet(ResourceSet&&) noexcept = default;

ResourceSet& ResourceSet::operator=(ResourceSet&&) noexcept = default;

void ResourceSet::SetSampler(uint stringID, Sampler& samp) { sampMap.SetResource(stringID, &samp); }

void ResourceSet::SetSRV(uint stringID, IShaderResource& srv) { textures.SetResource(stringID, &srv); }

void ResourceSet::SetUAV(uint stringID, IUnorderedAccess& uav) { rwTextures.SetResource(stringID, &uav); }

void ResourceSet::SetConstant(uint stringID, const Span<byte>& newValue) { constants.SetValue(stringID, newValue); }

const IDynamicArray<ResView<Sampler>>& ResourceSet::GetSamplers() const { return sampMap.data; }

const IDynamicArray<ResView<IShaderResource>>& ResourceSet::GetSRVs() const { return textures.data; }

const IDynamicArray<ResView<IUnorderedAccess>>& ResourceSet::GetUAVs() const { return rwTextures.data; }

uint ResourceSet::GetConstantCount() const { return (uint)constants.constants.GetLength(); }

void ResourceSet::ConstantGroup::SetValue(uint stringID, const Span<byte>& newValue)
{
	const auto& it = stringConstMap.find(stringID);

	if (it != stringConstMap.end()) // Update value
	{
		const Constant& value = constants[it->second];
		WV_ASSERT_MSG(newValue.GetLength() == value.size, "Constant size does not match existing value");
		memcpy(&data[value.offset], newValue.GetData(), newValue.GetLength());
	}
	else // Add new value
	{
		const uint index = (uint)constants.GetLength();
		const uint offset = (uint)data.GetLength();
		const uint size = (uint)newValue.GetLength();

		// Add constant descriptor
		constants.EmplaceBack(stringID, size, offset);
		// Map stringID -> constant desc index
		stringConstMap.emplace(stringID, index);
		// Write value to contiguous vector buffer
		data.Reserve(offset + size);
		std::copy(newValue.begin(), newValue.end(), std::back_inserter(data));
	}
}

void ResourceSet::ConstantGroup::Clear()
{
	data.Clear();
	constants.Clear();
	stringConstMap.clear();
}

const IDynamicArray<Span<byte>>& ResourceSet::GetMappedConstants(const ConstantGroupMap& map) const
{
	return map.GetData(constants);
}
