#pragma once
#include "D3D11/dev/ResourceBase.hpp"
#include "DynamicCollections.hpp"

namespace Replica::D3D11
{
	class BufferBase : public ResourceBase
	{
	public:
		const ResourceTypes type;
		const ResourceUsages usage;
		const ResourceAccessFlags cpuAccess;

		ID3D11Buffer* Get() { return pBuf.Get(); };

		ID3D11Buffer** GetAddressOf() { return pBuf.GetAddressOf(); };

		ID3D11Resource* GetResource() { return Get(); };

		ID3D11Resource** GetResAddress() { return reinterpret_cast<ID3D11Resource**>(GetAddressOf()); };

	protected:
		ComPtr<ID3D11Buffer> pBuf;

		BufferBase(ResourceTypes type, 
			ResourceUsages usage, 
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const void* data, 
			const UINT byteSize);

		template<typename T>
		BufferBase(ResourceTypes type, 
			ResourceUsages usage, 
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const IDynamicCollection<T>& data) :
			BufferBase(type, usage, cpuAccess, device, data.GetPtr(), (UINT)data.GetSize())
		{ }

		void CreateBuffer(const void* data, const UINT byteSize, ID3D11Device* pDevice);

		void GetBufferDesc(const void* data, const UINT byteSize, D3D11_BUFFER_DESC& desc, D3D11_SUBRESOURCE_DATA& resDesc);
	};
}