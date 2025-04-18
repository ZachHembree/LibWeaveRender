#pragma once
#include "../InternalD3D11.hpp"
#include "WeaveUtils/Utils.hpp"

namespace Weave::D3D11
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
	enum class ResourceBindFlags
	{
		None = 0,
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

		RWTexture = ShaderResource | RenderTarget | UnorderedAccess
	};

	BITWISE_ALL(ResourceBindFlags, UINT)

	/// <summary>
	/// Specifies types of CPU access allowed for mappable resources
	/// </summary>
	enum class ResourceAccessFlags : UINT
	{
		None = 0u,
		Write = D3D11_CPU_ACCESS_WRITE,
		Read = D3D11_CPU_ACCESS_READ,
		ReadWrite = Read | Write
	};

	BITWISE_ALL(ResourceAccessFlags, UINT)

	enum class DSClearFlags : int
	{
		None = 0,
		Depth = D3D11_CLEAR_DEPTH,
		Stencil = D3D11_CLEAR_STENCIL,
		DepthStencil = Depth | Stencil
	};

	BITWISE_ALL(DSClearFlags, int)

}