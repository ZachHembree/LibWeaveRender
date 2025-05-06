#pragma once
#include "WeaveUtils/DataBufferHandle.hpp"
#include "ResourceBase.hpp"
#include "../CommonTypes.hpp"

namespace Weave::D3D11
{
	class CtxImm;
	class CtxBase;

	/// <summary>
	/// Provides temporary access to a mapped resource buffer as a 1D contiguous span.
	/// Must be returned to the owning resource after use.
	/// </summary>
	class MappedBufferHandle : public DataBufferSpan<IBuffer, byte>
	{
	public:
		MappedBufferHandle();

		MappedBufferHandle(MappedBufferHandle&&) noexcept;

		MappedBufferHandle& operator=(MappedBufferHandle&&) noexcept;

		/// <summary>
		/// Returns true if the underlying buffer supports CPU read access
		/// </summary>
		bool GetCanRead() const;

		/// <summary>
		/// Returns true if the underlying buffer supports CPU write access
		/// </summary>
		bool GetCanWrite() const;

		/// <summary>
		/// Returns the offset in bytes between each row. In a 2D texture, this corresponds to a
		/// line. This value may be larger than the row itself due to padding.
		/// </summary>
		uint GetRowPitch() const;

		/// <summary>
		/// Returns the offset in bytes between each depth slice. In a 3D texture, this corresponds
		/// to the offset between 2D textures in the parent.
		/// </summary>
		uint GetDepthPitch() const;

		/// <summary>
		/// Returns the maximum extent of the buffer in bytes, including padding between rows and slices.
		/// </summary>
		uint GetByteSize() const;

	protected:
		friend CtxImm;
		MappedSubresource msr;
		uint subresIndex;	

		MappedBufferHandle(IBuffer& parent, MappedSubresource msr);
	};
}