#pragma once
#include "WeaveUtils/DynamicCollections.hpp"
#include "ResourceBase.hpp"
#include "../D3D11Utils.hpp"

namespace Weave::D3D11
{
	class CtxBase;

	/// <summary>
	/// Abstract base for GPU buffer types
	/// </summary>
	class BufferBase : public ResourceBase, public virtual IBuffer
	{
	public:
		ID3D11Buffer* GetBuffer();

		ID3D11Resource* GetResource() const override;

		/// <summary>
		/// Returns the total size of the buffer in bytes.
		/// </summary>
		uint GetSize() const;

		/// <summary>
		/// Returns the dimensions of the underlying buffer. Non-applicable dimensions are 
		/// always set to 1. For 1D buffers, Y == 1 and Z == 1.
		/// </summary>
		uivec3 GetDimensions() const override;

		/// <summary>
		/// Specifies how/if the CPU accesses the buffer
		/// </summary>
		ResourceUsages GetUsage() const;

		/// <summary>
		/// Indicates D3D11 buffer subtype, e.g. vertex, index, constant, etc.
		/// </summary>
		ResourceBindFlags GetBindFlags() const;

		/// <summary>
		/// Specifies CPU access rules for mappable resources
		/// </summary>
		ResourceAccessFlags GetAccessFlags() const;

		/// <summary>
		/// Writes the given data to the buffer. Writing to an unsupported buffer type will throw
		/// an exception.
		/// </summary>
		void SetRawData(CtxBase& ctx, const IDynamicArray<byte>& data);

		operator ID3D11Buffer* () { return GetBuffer(); }

	protected:
		DECL_MOVE_ONLY(BufferBase);

		BufferDesc desc;
		UniqueComPtr<ID3D11Buffer> pBuf;

		template<typename T>
		BufferBase(
			ResourceBindFlags type,
			ResourceUsages usage,
			ResourceAccessFlags cpuAccess,
			Device& device,
			const IDynamicArray<T>& data) :
			BufferBase(type, usage, cpuAccess, device, data.GetData(), (uint)GetArrSize(data))
		{ }

		BufferBase();

		BufferBase(
			ResourceBindFlags type, 
			ResourceUsages usage, 
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const void* data, 
			uint byteSize,
			uint miscFlags = 0,
			uint structStride = 0);

		virtual ~BufferBase();
	};
}