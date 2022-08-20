#pragma once
#include "D3D11/dev/ResourceBase.hpp"
#include "DynamicCollections.hpp"

namespace Replica::D3D11
{
	class BufferBase : public ResourceBase
	{
	public:
		ID3D11Buffer* Get() { return pBuf.Get(); };

		ID3D11Buffer** GetAddressOf() { return pBuf.GetAddressOf(); };

		ID3D11Resource* GetResource() { return Get(); };

		ID3D11Resource** GetResAddress() { return reinterpret_cast<ID3D11Resource**>(GetAddressOf()); };

		/// <summary>
		/// Returns the size of the buffer in bytes
		/// </summary>
		UINT GetSize() { return byteSize; }

	protected:
		ResourceTypes type;
		ResourceUsages usage;
		ResourceAccessFlags cpuAccess;
		ComPtr<ID3D11Buffer> pBuf;
		UINT byteSize;

		template<typename T>
		BufferBase(ResourceTypes type,
			ResourceUsages usage,
			ResourceAccessFlags cpuAccess,
			Device& device,
			const IDynamicCollection<T>& data) :
			BufferBase(type, usage, cpuAccess, device, data.GetPtr(), (UINT)data.GetSize())
		{ }

		BufferBase();

		BufferBase(ResourceTypes type, 
			ResourceUsages usage, 
			ResourceAccessFlags cpuAccess, 
			Device& device, 
			const void* data, 
			const UINT byteSize);

		BufferBase(BufferBase&& other) noexcept;

		BufferBase& operator=(BufferBase&& other) noexcept;

		void CreateBuffer(const void* data, const UINT byteSize, ID3D11Device* pDevice);
	
		void UpdateMapUnmap(const void* data, Context& ctx);
	};
}