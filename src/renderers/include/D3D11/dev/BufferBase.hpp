#pragma once
#include <d3d11.h>
#include "DynamicCollections.hpp"
#include "D3D11/dev/DeviceChild.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Specifies how a buffer will be used
	/// </summary>
	enum class BufferUsages
	{
		/// <summary>
		/// R/W Access required for GPU
		/// </summary>
		Default = D3D11_USAGE_DEFAULT,

		/// <summary>
		/// Read-only GPU resource. Cannot be accessed by CPU.
		/// </summary>
		Immutable = D3D11_USAGE_IMMUTABLE,

		/// <summary>
		/// Read-only GPU access; write-only CPU access.
		/// </summary>
		Dynamic = D3D11_USAGE_DYNAMIC,

		/// <summary>
		/// Resource supports transfer from GPU to CPU
		/// </summary>
		Staging = D3D11_USAGE_STAGING
	};

	/// <summary>
	/// Supported buffer types
	/// </summary>
	enum class BufferTypes
	{
		Vertex = D3D11_BIND_VERTEX_BUFFER, 
		Index = D3D11_BIND_INDEX_BUFFER,
		Constant = D3D11_BIND_CONSTANT_BUFFER
	};

	/// <summary>
	/// Specifies types of CPU access allowed for a resource
	/// </summary>
	enum class BufferAccessFlags
	{
		None = 0u,
		Write = D3D11_CPU_ACCESS_WRITE,
		Read = D3D11_CPU_ACCESS_READ
	};

	class BufferBase : public DeviceChild
	{
	public:
		const BufferTypes type;
		const BufferUsages usage;
		const BufferAccessFlags cpuAccess;

		ID3D11Buffer* Get() { return pBuf.Get(); };

		ID3D11Buffer** GetAddressOf() { return pBuf.GetAddressOf(); };

	protected:
		ComPtr<ID3D11Buffer> pBuf;

		BufferBase(BufferTypes type, 
			BufferUsages usage, 
			BufferAccessFlags cpuAccess, 
			Device& device, 
			const void* data, 
			const UINT byteSize);

		template<typename T>
		BufferBase(BufferTypes type, 
			BufferUsages usage, 
			BufferAccessFlags cpuAccess, 
			Device& device, 
			const IDynamicCollection<T>& data) :
			BufferBase(type, usage, cpuAccess, device, data.GetPtr(), (UINT)data.GetSize())
		{ }

		void CreateBuffer(const void* data, const UINT byteSize, ID3D11Device* pDevice);

		void GetBufferDesc(const void* data, const UINT byteSize, D3D11_BUFFER_DESC& desc, D3D11_SUBRESOURCE_DATA& resDesc);
	};
}