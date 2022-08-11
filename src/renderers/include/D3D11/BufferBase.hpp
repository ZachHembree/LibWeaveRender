#pragma once
#include "MinWindow.hpp"
#include "GfxException.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <wrl.h>
#include <math.h>
#include "D3D11/Device.hpp"

namespace Replica::D3D11
{
	class Device;

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

	class BufferBase
	{
	public:
		const BufferTypes type;
		const BufferUsages usage;
		const BufferAccessFlags cpuAccess;

		ID3D11Buffer* Get() const { return pBuf.Get(); }

		ID3D11Buffer** GetAddressOf() { return pBuf.GetAddressOf(); }

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuf;

		BufferBase(BufferTypes type, BufferUsages usage, BufferAccessFlags cpuAccess) :
			type(type),
			usage(usage),
			cpuAccess(cpuAccess)
		{ }

		template<typename T>
		BufferBase(BufferTypes type, 
			BufferUsages usage, 
			BufferAccessFlags cpuAccess, 
			const Device& device, 
			const DynamicArrayBase<T>& data) :
			BufferBase(type, usage, cpuAccess)
		{
			CreateBuffer(data, device.Get());
		}

		template<typename T>
		BufferBase(BufferTypes type, 
			BufferUsages usage, 
			BufferAccessFlags cpuAccess,
			const Device& device, 
			const std::vector<T>& data) :
			BufferBase(type, usage, cpuAccess)
		{
			CreateBuffer(data, device.Get());
		}

		BufferBase(const BufferBase&) = delete;
		BufferBase(BufferBase&&) = delete;
		BufferBase& operator=(const BufferBase&) = delete;
		BufferBase& operator=(BufferBase&&) = delete;

		template<typename T>
		void CreateBuffer(const DynamicArrayBase<T>& data, ID3D11Device* pDevice)
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA resDesc;

			GetBufferDesc(data, desc, resDesc);
			GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, &resDesc, &pBuf));
		}

		template<typename T>
		void CreateBuffer(const std::vector<T>& data, ID3D11Device* pDevice)
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA resDesc;

			GetBufferDesc(data, desc, resDesc);
			GFX_THROW_FAILED(pDevice->CreateBuffer(&desc, &resDesc, &pBuf));
		}

		template<typename T>
		void GetBufferDesc(const DynamicArrayBase<T>& data, D3D11_BUFFER_DESC& desc, 
			D3D11_SUBRESOURCE_DATA& resDesc)
		{
			desc = {};
			desc.Usage = (D3D11_USAGE)usage;
			desc.BindFlags = (UINT)type;
			desc.ByteWidth = (UINT)data.GetSize();
			desc.CPUAccessFlags = (UINT)cpuAccess;
			desc.MiscFlags = 0;

			resDesc.pSysMem = data.GetPtr();
			resDesc.SysMemPitch = 0;
			resDesc.SysMemSlicePitch = 0;
		}

		template<typename T>
		void GetBufferDesc(const std::vector<T>& data, D3D11_BUFFER_DESC& desc,
			D3D11_SUBRESOURCE_DATA& resDesc)
		{
			desc = {};
			desc.Usage = (D3D11_USAGE)usage;
			desc.BindFlags = (UINT)type;
			desc.ByteWidth = (UINT)data.size() * sizeof(T);
			desc.CPUAccessFlags = (UINT)cpuAccess;
			desc.MiscFlags = 0;

			resDesc.pSysMem = data.data();
			resDesc.SysMemPitch = 0;
			resDesc.SysMemSlicePitch = 0;
		}
	};
}