#pragma once
#include <d3d11.h>
#include "D3D11/Resources/DeviceChild.hpp"

namespace Replica::D3D11
{
	/// <summary>
	/// Specifies how a buffer will be used
	/// </summary>
	enum class ResourceUsages
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
	enum class ResourceTypes
	{
		Vertex = D3D11_BIND_VERTEX_BUFFER,
		Index = D3D11_BIND_INDEX_BUFFER,
		Constant = D3D11_BIND_CONSTANT_BUFFER,
		ShaderResource = D3D11_BIND_SHADER_RESOURCE,
		StreamOutput = D3D11_BIND_STREAM_OUTPUT,
		RenderTarget = D3D11_BIND_RENDER_TARGET,
		DepthStencil = D3D11_BIND_DEPTH_STENCIL,
		UnorderedAccess = D3D11_BIND_UNORDERED_ACCESS,
		Decoder = D3D11_BIND_DECODER,
		VideoEncoder = D3D11_BIND_VIDEO_ENCODER,
	};

	BITWISE_ALL(ResourceTypes, UINT)

	/// <summary>
	/// Specifies types of CPU access allowed for a resource
	/// </summary>
	enum class ResourceAccessFlags : UINT
	{
		None = 0u,
		Write = D3D11_CPU_ACCESS_WRITE,
		Read = D3D11_CPU_ACCESS_READ
	};

	BITWISE_ALL(ResourceAccessFlags, UINT)

	class ResourceBase : public DeviceChild
	{
	public:
		virtual ID3D11Resource* GetResource() = 0;

		virtual ID3D11Resource** const GetResAddress() = 0;

	protected:
		ResourceBase() { }

		ResourceBase(Device& dev) : DeviceChild(dev) {}

	};
}