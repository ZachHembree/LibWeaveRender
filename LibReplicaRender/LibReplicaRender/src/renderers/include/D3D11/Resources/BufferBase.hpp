#pragma once
#include "ResourceBase.hpp"
#include "ReplicaD3D11.hpp"

namespace Replica::D3D11
{
	class BufferBase : public ResourceBase
	{
	public:
		ID3D11Buffer* Get();

		ID3D11Buffer** const GetAddressOf();

		ID3D11Resource* GetResource();

		ID3D11Resource** const GetResAddress();

		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		UINT GetSize() const;

		ResourceUsages GetUsage() const;

		ResourceBindFlags GetBindFlags() const;

		ResourceAccessFlags GetAccessFlags() const;

		void SetData(Context& ctx, const void* data);

	protected:
		D3D11_BUFFER_DESC desc;
		ComPtr<ID3D11Buffer> pBuf;

		template<typename T>
		BufferBase(
			ResourceBindFlags type,
			ResourceUsages usage,
			ResourceAccessFlags cpuAccess,
			Device& device,
			const IDynamicArray<T>& data) :
			BufferBase(type, usage, cpuAccess, device, data.GetPtr(), (UINT)GetArrSize(data))
		{ }

		BufferBase();

		BufferBase(
			ResourceBindFlags type, 
			ResourceUsages usage, 
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const void* data, 
			const UINT byteSize);

		void UpdateSubresource(Context& ctx, const void* data);

		void UpdateMapUnmap(Context& ctx, const void* data);
	};
}