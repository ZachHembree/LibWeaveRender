#include "D3D11/dev/DeviceChild.hpp"
#include "D3D11/dev/InputLayout.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/ConstantMap.hpp"

using namespace Replica;
using namespace Replica::D3D11;

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

/// <summary>
/// Writes contents of the constant map to the given constant buffer
/// </summary>
void ConstantMap::UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx)
{
	GFX_ASSERT(cb.GetSize() >= data.GetSize(), "Destination constant buffer cannot be smaller than the source.");
	cb.SetData(data.GetPtr(), ctx);
}

/// <summary>
/// Sets member with the given name to the value given
/// </summary>
void ConstantMap::SetMember(WSTR name, const byte* src, const type_info& type)
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

/// <summary>
/// Adds a new constant entry with the given name and type to the end
/// of the map definition.
/// </summary>
void ConstantMapDef::Add(WSTR name, const type_info& type, const size_t stride)
{
	members.emplace_back(ConstantDef(name, type, stride));
	this->stride += stride;
}

/// <summary>
/// Clears the contents of the initializer
/// </summary>
void ConstantMapDef::Clear()
{
	members.erase(members.begin(), members.end());
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

ConstantDef::ConstantDef() : name(nullptr), type(typeid(void*)), stride(0)
{ }

ConstantDef::ConstantDef(WSTR name, const type_info& type, const size_t stride) :
	name(name),
	type(type),
	stride(stride)
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