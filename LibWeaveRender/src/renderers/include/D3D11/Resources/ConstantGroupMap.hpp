#pragma once
#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveEffects/ShaderDataHandles.hpp"
#include "ResourceSet.hpp"
#include "WeaveUtils/Span.hpp"

namespace Weave::D3D11
{
	using Effects::ConstBufDefHandle;
	using Effects::ConstDef;

	class ConstantBuffer;

	/// <summary>
	/// Defines the layout of a group of constant buffers in a particular order
	/// </summary>
	class ConstantGroupMap
	{
	public:
		using GroupData = ConstantGroupBuffer;
		using ConstantData = ConstantDesc;
		using Data = IDynamicArray<Span<byte>>;

		MAKE_MOVE_ONLY(ConstantGroupMap)

		/// <summary>
		/// Default constructor. Initializes an empty constant map.
		/// </summary>
		ConstantGroupMap();

		/// <summary>
		/// Constructs a constant map using the provided constant buffer layout.
		/// The layout defines the total size and the metadata (offset and size)
		/// for each constant buffer and constant stored in the map.
		/// </summary>
		ConstantGroupMap(const std::optional<ConstBufGroupHandle>& layout);

		/// <summary>
		/// Reads unordered constants from ResourceSet into internal fixed layout 
		/// buffers and returns a reference.
		/// </summary>
		const Data& GetData(const GroupData& src) const;
		
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
		using ConstantMap = UniqueArray<ConstDef>;

		// Shared contiguous buffer
		UniqueArray<byte> cbufData;
		// Parallel cbuf data - arr[x] -> buffer data/const desc
		mutable UniqueArray<Span<byte>> cbufSpans;
		UniqueArray<ConstantMap> cbufMaps;

		uint totalSize;
	};
}