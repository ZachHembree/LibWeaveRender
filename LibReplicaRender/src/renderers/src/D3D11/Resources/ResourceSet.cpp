#include "pch.hpp"
#include "D3D11/Resources/ResourceSet.hpp"
#include "D3D11/Resources/ConstantGroupMap.hpp"

using namespace Replica;
using namespace Replica::D3D11;

template<typename T>
using ResView = ResourceSet::ResView<T>;

ResourceSet::ResourceSet() = default;

ResourceSet::ResourceSet(ResourceSet&&) noexcept = default;

ResourceSet& ResourceSet::operator=(ResourceSet&&) noexcept = default;

void ResourceSet::SetSampler(uint stringID, const ComPtr<ID3D11SamplerState>& pSamp) { sampMap.SetResource(stringID, pSamp.Get()); }

void ResourceSet::SetSRV(uint stringID, const ComPtr<ID3D11ShaderResourceView>& pSRV) { textures.SetResource(stringID, pSRV.Get()); }

void ResourceSet::SetUAV(uint stringID, const ComPtr<ID3D11UnorderedAccessView>& pUAV) { rwTextures.SetResource(stringID, pUAV.Get()); }

void ResourceSet::SetConstant(uint stringID, const byte* pSrc, const uint size) { constants.SetValue(stringID, pSrc, size); }

const IDynamicArray<ResView<ID3D11SamplerState>>& ResourceSet::GetSamplers() const { return sampMap.data; }

const IDynamicArray<ResView<ID3D11ShaderResourceView>>& ResourceSet::GetSRVs() const { return textures.data; }

const IDynamicArray<ResView<ID3D11UnorderedAccessView>>& ResourceSet::GetUAVs() const { return rwTextures.data; }

void ResourceSet::ConstantGroup::SetValue(uint stringID, const byte* pValue, uint size)
{
	const auto& it = stringConstMap.find(stringID);

	if (it != stringConstMap.end()) // Update value
	{
		const Constant& value = constants[it->second];
		REP_ASSERT_MSG(size == value.size, "Constant size does not match existing value");
		byte* pDst = &data[value.offset];
		memcpy(pDst, pValue, size);
	}
	else // Add new value
	{
		const uint index = (uint)constants.GetLength();
		const uint offset = (uint)data.GetLength();
		constants.emplace_back(stringID, size, offset);
		stringConstMap.emplace(stringID, index);
		data.reserve(offset + size);

		for (size_t i = 0; i < size; i++)
			data.emplace_back(pValue[i]);
	}
}

void ResourceSet::ConstantGroup::Clear()
{
	data.clear();
	constants.clear();
	stringConstMap.clear();
}

const IDynamicArray<Span<byte>>& ResourceSet::GetMappedConstants(const ConstantGroupMap& map) const
{
	if (map.GetTotalSize() > cbufData.GetLength())
		cbufData = UniqueArray<byte>(map.GetTotalSize());

	bufferSpans.clear();
	bufferSpans.reserve(map.GetBufferCount());

	byte* pStart = &cbufData[0];

	for (uint i = 0; i < map.GetBufferCount(); i++)
	{
		const uint size = map.GetBufferSize(i);
		bufferSpans.emplace_back(pStart, size);
		pStart += size;
	}

	map.SetData(constants, bufferSpans);
	return bufferSpans;
}
