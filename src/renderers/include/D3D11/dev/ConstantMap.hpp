#pragma once
#include "D3D11/D3DUtils.hpp"
#include <unordered_map>
#include <typeinfo>

namespace Replica::D3D11
{
	struct ConstantDef;
	class ConstantMapDef;
	class ConstantBuffer;

	/// <summary>
	/// Metadata object used to define a constant buffer whose size and layout are determined
	/// on initialization.
	/// </summary>
	class ConstantMap : UniqueObjBase
	{
	public:		
		ConstantMap();

		ConstantMap(const ConstantMapDef& layout);

		ConstantMap(ConstantMap&& other) noexcept;

		ConstantMap& operator=(ConstantMap&& other) noexcept;

		/// <summary>
		/// Writes contents of the constant map to the given constant buffer
		/// </summary>
		void UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx);

		/// <summary>
		/// Returns true if a member with the given name is registered to the map
		/// </summary>
		bool GetMemberExists(wstring_view name);

		/// <summary>
		/// Sets member with the given name to the value given
		/// </summary>
		template<typename T>
		void SetMember(wstring_view name, const T& value)
		{
			if (GetMemberExists(name))
				SetMember(name, reinterpret_cast<const byte*>(&value), typeid(T));
		}
		
		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		size_t GetStride();

		/// <summary>
		/// Defines the properties and location of a constant in a ConstantMap
		/// </summary>
		struct MapEntry
		{
			const type_info& type;
			const size_t stride;
			const size_t offset;

			MapEntry();

			MapEntry(const type_info& type, const size_t stride, const size_t offset);

			MapEntry(const MapEntry& other);

			MapEntry& operator=(MapEntry& other);
		};

	private:
		UniqueArray<byte> data;
		std::unordered_map<wstring_view, MapEntry> defMap;
		size_t stride;

		ConstantMap(const ConstantMap& other);

		ConstantMap& operator=(const ConstantMap& other);

		void SetMember(wstring_view name, const byte* src, const type_info& type);
	};

	/// <summary>
	/// Collection for initializing a new ConstantMap
	/// </summary>
	class ConstantMapDef : UniqueObjBase
	{
	public:
		ConstantMapDef();

		ConstantMapDef(const IDynamicCollection<ConstantDef>& definition);;

		ConstantMapDef(const std::initializer_list<ConstantDef>& definition);

		ConstantMapDef(const ConstantMapDef& other) noexcept;

		ConstantMapDef& operator=(const ConstantMapDef& other) noexcept;

		ConstantMapDef(ConstantMapDef&& other) noexcept;

		ConstantMapDef& operator=(ConstantMapDef&& other) noexcept;

		/// <summary>
		/// Adds a new constant entry with the given name and type to the end
		/// of the map definition.
		/// </summary>
		template<typename T>
		void Add(wstring_view name) { Add(name, typeid(T), sizeof(T)); }

		/// <summary>
		/// Clears the contents of the initializer
		/// </summary>
		void Clear();

		/// <summary>
		/// Returns the size of the buffer defined by the definition, in bytes.
		/// </summary>
		size_t GetStride() const;

		/// <summary>
		/// Returns a list of the constants defined by the map definition
		/// </summary>
		const IDynamicCollection<ConstantDef>& GetMembers() const;

	private:
		UniqueVector<ConstantDef> members;
		size_t stride;

		/// <summary>
		/// Adds a new constant entry with the given name and type to the end
		/// of the map definition.
		/// </summary>
		void Add(wstring_view name, const type_info& type, const size_t stride);
	};

	/// <summary>
	/// Defines the properties of an individual constant in a ConstantMapDef
	/// </summary>
	struct ConstantDef
	{
		wstring_view name;
		const type_info& type;
		const size_t stride;

		ConstantDef();

		ConstantDef(wstring_view name, const type_info& type, const size_t stride);

		ConstantDef(const ConstantDef& other);

		ConstantDef& operator=(const ConstantDef& other);

		template<typename T>
		static ConstantDef Get(wstring_view name)
		{
			return ConstantDef(name, typeid(T), sizeof(T));
		}
	};
}