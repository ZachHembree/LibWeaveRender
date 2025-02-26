#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Context.hpp"
#include "D3D11/Resources/ConstantMap.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"

using namespace Replica;
using namespace Replica::Effects;
using namespace Replica::D3D11;

ConstantMap::ConstantMap() : stride(0)
{ }

ConstantMap::ConstantMap(const ConstBufLayout& layout) :
	stride(GetAlignedByteSize(layout.size, g_ConstantBufferAlignment)),
	data(stride)
{
	const IDynamicArray<ConstDef>& members = layout.members;
	size_t offset = 0;
	defMap.reserve(members.GetLength());

	for (int i = 0; i < members.GetLength(); i++)
	{
		const ConstDef& member = members[i];
		const MapEntry entry(member.size, offset);

		defMap.emplace(member.name, entry);
		offset += member.size;
	}
}

/// <summary>
/// Writes contents of the constant map to the given constant buffer
/// </summary>
void ConstantMap::UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx) const
{
	GFX_ASSERT(cb.GetSize() >= GetArrSize(data), "Destination constant buffer cannot be smaller than the source.");
	cb.SetData(ctx, data.GetPtr());
}

/// <summary>
/// Returns true if a member with the given name is registered to the map
/// </summary>
bool ConstantMap::GetMemberExists(string_view name) const
{
	auto pair = defMap.find(name);
	return pair != defMap.end();
}

/// <summary>
/// Sets member with the given name to the value given
/// </summary>
void ConstantMap::SetMember(string_view name, const byte* src, const size_t size)
{
	const auto itr = defMap.find(name);
	GFX_ASSERT(itr != defMap.end(), "Named constant not in buffer definition.");
	
	const MapEntry& entry = itr->second;
	GFX_ASSERT(size == entry.stride, "Shader constant size does not match data given.");

	// Calculate pointer to entry
	byte* dst = this->data.GetPtr();
	dst += entry.offset;

	// Write entry to array
	memcpy(dst, src, entry.stride);
}

/// <summary>
/// Writes the given data to the buffer
/// </summary>
void ConstantMap::SetData(const byte* pData, size_t size)
{
	memcpy_s(data.GetPtr(), data.GetLength(), pData, size);
}

/// <summary>
/// Returns the size of the buffer in bytes
/// </summary>
size_t ConstantMap::GetBufferSize() const { return stride; }

ConstantMap::MapEntry::MapEntry() : stride(0), offset(0)
{ }

ConstantMap::MapEntry::MapEntry(const size_t stride, const size_t offset) :
	stride(stride),
	offset(offset)
{ }

ConstantMap::MapEntry::MapEntry(const MapEntry& other) :
	stride(other.stride),
	offset(other.offset)
{ }

ConstantMap::MapEntry& ConstantMap::MapEntry::operator=(MapEntry& other)
{
	memcpy(this, &other, sizeof(MapEntry));
	return *this;
}