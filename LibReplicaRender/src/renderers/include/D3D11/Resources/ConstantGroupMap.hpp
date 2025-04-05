#pragma once
#include "ReplicaUtils/Utils.hpp"
#include "ReplicaEffects/ShaderDataHandles.hpp"
#include "ResourceSet.hpp"
#include "ReplicaUtils/Span.hpp"

namespace Replica::D3D11
{
	using Effects::ConstBufDefHandle;

	class Context;
	class ConstantBuffer;

	/// <summary>
	/// Defines the layout of a group of constant buffers in a particular order
	/// </summary>
	class ConstantGroupMap
	{
	public:
		using GroupData = ResourceSet::ConstantGroup;
		using ConstantData = ResourceSet::Constant;

		MAKE_MOVE_ONLY(ConstantGroupMap)

		/// <summary>
		/// Default constructor. Initializes an empty constant map.
		/// </summary>
		ConstantGroupMap();

		/// <summary>
		/// Constructs a ConstantMap using the provided constant buffer layout.
		/// The layout defines the total size and the metadata (offset and size)
		/// for each constant stored in the map.
		/// </summary>
		ConstantGroupMap(const std::optional<ConstBufGroupHandle>& layout);

		/// <summary>
		/// Returns true if a member with the given string ID is registered to the map
		/// </summary>
		bool GetMemberExists(uint nameID, sint buffer) const;

		void SetData(const GroupData& src, IDynamicArray<Span<byte>>& dstBufs) const;
		
		/// <summary>
		/// Returns the combined size, in bytes, of all CBs in the group
		/// </summary>
		uint GetTotalSize() const;

		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		uint GetBufferSize(sint buffer) const;

		/// <summary>
		/// Returns the number of buffers in the group
		/// </summary>
		uint GetBufferCount() const;

	private:
		struct Constant
		{
			MAKE_DEF_MOVE_COPY(Constant)

			const uint size;
			const uint offset;

			Constant();

			Constant(const uint size, const uint offset);
		};

		struct ConstantMap
		{ 
			uint index;
			uint size;
			// stringID -> Constant
			std::unordered_map<uint, Constant> constMap;
		};

		UniqueArray<ConstantMap> group;
		uint size;
	};
}