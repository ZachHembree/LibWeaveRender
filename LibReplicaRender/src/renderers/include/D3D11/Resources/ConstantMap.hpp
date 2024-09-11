#pragma once
#include "ReplicaUtils.hpp"
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
	class ConstantMap
	{
	public:		
		ConstantMap();

		ConstantMap(const ConstantMapDef& layout);

		ConstantMap(ConstantMap&&) = default;

		ConstantMap& operator=(ConstantMap&&) = default;

		/// <summary>
		/// Writes contents of the constant map to the given constant buffer
		/// </summary>
		void UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx);

		/// <summary>
		/// Returns true if a member with the given name is registered to the map
		/// </summary>
		bool GetMemberExists(string_view name);

		/// <summary>
		/// Sets member with the given name to the value given
		/// </summary>
		template<typename T>
		void SetMember(string_view name, const T& value)
		{
			if (GetMemberExists(name))
				SetMember(name, reinterpret_cast<const byte*>(&value), sizeof(T));
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
			const size_t stride;
			const size_t offset;

			MapEntry();

			MapEntry(const size_t stride, const size_t offset);

			MapEntry(const MapEntry& other);

			MapEntry& operator=(MapEntry& other);
		};

	private:
		UniqueArray<byte> data;
		std::unordered_map<string_view, MapEntry> defMap;
		size_t stride;

		ConstantMap(const ConstantMap& other);

		ConstantMap& operator=(const ConstantMap& other);

		void SetMember(string_view name, const byte* src, const size_t size);
	};

	/// <summary>
	/// Collection for initializing a new ConstantMap
	/// </summary>
	class ConstantMapDef
	{
	public:
		ConstantMapDef();

		ConstantMapDef(const IDynamicArray<ConstantDef>& definition);;

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
		void Add(string_view name) { Add(name, sizeof(T)); }

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
		const IDynamicArray<ConstantDef>& GetMembers() const;

	private:
		UniqueVector<ConstantDef> members;
		size_t stride;

		/// <summary>
		/// Adds a new constant entry with the given name and type to the end
		/// of the map definition.
		/// </summary>
		void Add(string_view name, const size_t stride);
	};

	/// <summary>
	/// Defines the properties of an individual constant in a ConstantMapDef
	/// </summary>
	struct ConstantDef
	{
		string_view name;
		const size_t stride;

		ConstantDef();

		ConstantDef(string_view name, const size_t stride);

		ConstantDef(const ConstantDef& other);

		ConstantDef& operator=(const ConstantDef& other);

		template<typename T>
		static ConstantDef Get(string_view name)
		{
			return ConstantDef(name, sizeof(T));
		}
	};
}