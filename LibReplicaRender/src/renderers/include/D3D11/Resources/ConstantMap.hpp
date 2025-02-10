#pragma once
#include "ReplicaUtils.hpp"
#include "ShaderLibGen/ShaderData.hpp"
#include <unordered_map>

namespace Replica::D3D11
{
	using Effects::ConstBufLayout;

	class ConstantBuffer;

	/// <summary>
	/// The ConstantMap class defines the layout and storage of a constant buffer.
    /// It uses a dynamically sized byte array to store constant values and an internal
    /// mapping from constant names (as string_view) to their corresponding offset and size.
	/// </summary>
	class ConstantMap
	{
	public:
		MAKE_MOVE_ONLY(ConstantMap)

		/// <summary>
		/// Default constructor. Initializes an empty constant map.
		/// </summary>
		ConstantMap();

		/// <summary>
		/// Constructs a ConstantMap using the provided constant buffer layout.
		/// The layout defines the total size and the metadata (offset and size)
		/// for each constant stored in the map.
		/// </summary>
		ConstantMap(const ConstBufLayout& layout);

		/// <summary>
		/// Writes the current contents of this constant map to the provided constant buffer.
		/// The destination constant buffer must be large enough to hold the data.
		/// </summary>
		void UpdateConstantBuffer(ConstantBuffer& cb, Context& ctx) const;

		/// <summary>
		/// Returns true if a member with the given name is registered to the map
		/// </summary>
		bool GetMemberExists(string_view name) const;

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
		/// Writes the given data to the buffer
		/// </summary>
		void SetData(const byte* pData, size_t size);
		
		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		size_t GetBufferSize() const;

	private:
		/// <summary>
		/// MapEntry holds metadata for a single constant within the buffer,
		/// including its size (stride) and its offset (in bytes) within the data array.
		/// These fields are immutable once a MapEntry is constructed.
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

		UniqueArray<byte> data;
		std::unordered_map<string_view, MapEntry> defMap;
		size_t stride;

		void SetMember(string_view name, const byte* src, const size_t size);
	};
}