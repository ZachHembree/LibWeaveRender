#include "ReplicaInternalD3D11.hpp"

using namespace Replica;
using namespace Replica::D3D11;

ConstantMap::ConstantMap() : stride(0)
{ }

ConstantMap::ConstantMap(const ConstantMapDef& layout) :
	stride(layout.GetStride()),
	data(layout.GetStride())
{
	const IDynamicCollection<ConstantDef>& members = layout.GetMembers();
	size_t offset = 0;
	defMap.reserve(members.GetLength());

	for (int i = 0; i < members.GetLength(); i++)
	{
		const ConstantDef& member = members[i];
		const MapEntry entry(member.type, member.stride, offset);

		defMap.emplace(member.name, entry);
		offset += member.stride;
	}
}

ConstantMap::ConstantMap(const ConstantMap& other) :
	data(other.data.GetCopy()),
	defMap(other.defMap),
	stride(other.stride)
{ }

ConstantMap& ConstantMap::operator=(const ConstantMap& other)
{
	this->data = other.data.GetCopy();
	this->defMap = std::unordered_map<wstring_view, MapEntry>(other.defMap);
	this->stride = other.stride;

	return *this;
}

/// <summary>
/// Writes contents of the constant map to the given constant buffer
/// </summary>
void ConstantMap::UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx)
{
	GFX_ASSERT(cb.GetSize() >= data.GetSize(), "Destination constant buffer cannot be smaller than the source.");
	cb.SetData(ctx, data.GetPtr());
}

/// <summary>
/// Returns true if a member with the given name is registered to the map
/// </summary>
bool ConstantMap::GetMemberExists(wstring_view name)
{
	auto pair = defMap.find(name);
	return pair != defMap.end();
}

/// <summary>
/// Sets member with the given name to the value given
/// </summary>
void ConstantMap::SetMember(wstring_view name, const byte* src, const type_info& type)
{
	const auto itr = defMap.find(name);
	GFX_ASSERT(itr != defMap.end(), "Named constant not in buffer definition.");
	
	const MapEntry& entry = itr->second;
	GFX_ASSERT(type == entry.type, "Shader constant type does not match data given.");

	// Calculate pointer to entry
	byte* dst = this->data.GetPtr();
	dst += entry.offset;

	// Write entry to array
	memcpy(dst, src, entry.stride);
}

/// <summary>
/// Returns the size of the buffer in bytes
/// </summary>
size_t ConstantMap::GetStride() { return stride; }

/// <summary>
/// Initializes a new map definition
/// </summary>
ConstantMapDef::ConstantMapDef() :
	stride(0)
{
	members.reserve(10);
}

ConstantMapDef::ConstantMapDef(const IDynamicCollection<ConstantDef>& definition) :
	members(definition),
	stride(0)
{
	for (ConstantDef& def : members)
		stride += def.stride;
}

ConstantMapDef::ConstantMapDef(const std::initializer_list<ConstantDef>& definition) :
	ConstantMapDef(UniqueArray(definition))
{ }

ConstantMapDef::ConstantMapDef(const ConstantMapDef& other) noexcept :
	members(other.members.GetCopy()),
	stride(other.stride)
{ }

ConstantMapDef::ConstantMapDef(ConstantMapDef&& other) noexcept :
	members(std::move(other.members)),
	stride(other.stride)
{ }

ConstantMapDef& ConstantMapDef::operator=(const ConstantMapDef& other) noexcept
{
	this->members = other.members.GetCopy();
	this->stride = other.stride;

	return *this;
}

ConstantMapDef& ConstantMapDef::operator=(ConstantMapDef&& other) noexcept
{
	this->members = std::move(other.members);
	this->stride = other.stride;

	return *this;
}

/// <summary>
/// Adds a new constant entry with the given name and type to the end
/// of the map definition.
/// </summary>
void ConstantMapDef::Add(wstring_view name, const type_info& type, const size_t stride)
{
	members.emplace_back(ConstantDef(name, type, stride));
	this->stride += stride;
}

/// <summary>
/// Clears the contents of the initializer
/// </summary>
void ConstantMapDef::Clear()
{
	members.clear();
	stride = 0;
}

/// <summary>
/// Returns a list of the constants defined by the map definition
/// </summary>
const IDynamicCollection<ConstantDef>& ConstantMapDef::GetMembers() const { return members; }

/// <summary>
/// Returns the size of the buffer defined by the definition, in bytes.
/// </summary>
size_t ConstantMapDef::GetStride() const { return stride; }

ConstantDef::ConstantDef() : name(L""), type(typeid(void*)), stride(0)
{ }

ConstantDef::ConstantDef(wstring_view name, const type_info& type, const size_t stride) :
	name(name),
	type(type),
	stride(GetAlignedByteSize(stride, 16))
{ }

ConstantDef::ConstantDef(const ConstantDef& other) :
	name(other.name),
	type(other.type),
	stride(other.stride)
{ }

ConstantDef& ConstantDef::operator=(const ConstantDef& other)
{
	memcpy(this, &other, sizeof(ConstantDef));
	return *this;
}

ConstantMap::MapEntry::MapEntry() : type(typeid(void*)), stride(0), offset(0)
{ }

ConstantMap::MapEntry::MapEntry(const type_info& type, const size_t stride, const size_t offset) :
	type(type),
	stride(stride),
	offset(offset)
{ }

ConstantMap::MapEntry::MapEntry(const MapEntry& other) :
	type(other.type),
	stride(other.stride),
	offset(other.offset)
{ }

ConstantMap::MapEntry& ConstantMap::MapEntry::operator=(MapEntry& other)
{
	memcpy(this, &other, sizeof(MapEntry));
	return *this;
}