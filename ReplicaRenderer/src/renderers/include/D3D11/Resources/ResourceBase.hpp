#pragma once
#pragma warning(disable: 4250)

#include <d3d11.h>
#include "D3D11/Resources/DeviceChild.hpp"
#include "D3D11/Resources/ResourceEnums.hpp"
#include "D3D11/Resources/Formats.hpp"

namespace Replica::D3D11
{
	class ResourceBase : public DeviceChild
	{
	public:
		virtual ID3D11Resource* GetResource() = 0;

		virtual ID3D11Resource** const GetResAddress() = 0;

	protected:
		ResourceBase() { }

		ResourceBase(Device& dev) : DeviceChild(dev) {}

	};

	/// <summary>
	/// Interface for types that can be bound as a shader resource
	/// </summary>
	class IShaderResource
	{
	public:
		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		virtual ID3D11ShaderResourceView* GetSRV() = 0;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		virtual ID3D11ShaderResourceView** const GetSRVAddress() = 0;
	};

	/// <summary>
	/// Interface for types that can be bound as Render Targets
	/// </summary>
	class IRenderTarget
	{
	public:
		/// <summary>
		/// Pointer to Render Target view interface
		/// </summary>
		virtual ID3D11RenderTargetView* GetRTV() = 0;

		/// <summary>
		/// Read-only pointer to pointer for Render Target view
		/// </summary>
		virtual ID3D11RenderTargetView** const GetAddressRTV() = 0;

		/// <summary>
		/// Clears the render target to the given color
		/// </summary>
		virtual void Clear(
			Context& ctx,
			vec4 color = vec4(0)
		) = 0;
	};

	/// <summary>
	/// Interface for types that can be used for unordered access
	/// </summary>
	class IUnorderedAccess
	{
	public:
		
		/// <summary>
		/// Returns pointer to UAV interface
		/// </summary>
		virtual ID3D11UnorderedAccessView* GetUAV() = 0;

		/// <summary>
		/// Returns pointer to UAV pointer field
		/// </summary>
		virtual ID3D11UnorderedAccessView** const GetAddressUAV() = 0;
	};

	class IDepthStencil
	{
	public:
		/// <summary>
		/// Returns pointer to depth stencil state interface
		/// </summary>
		virtual ID3D11DepthStencilState* GetState() = 0;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		virtual ID3D11DepthStencilView* GetDSV() = 0;

		/// <summary>
		/// Returns interface to depth-stencil view
		/// </summary>
		virtual ID3D11DepthStencilView** const GetDSVAddress() = 0;

		/// <summary>
		/// Clears the texture
		/// </summary>
		virtual void Clear(
			Context& ctx,
			DSClearFlags clearFlags = DSClearFlags::Depth,
			float depthClear = 1.0f,
			UINT8 stencilClear = 0
		) const = 0;
	};

	/// <summary>
	/// Interface for 2D Textures, without resource views
	/// </summary>
	class ITexture2DBase
	{
	public:
		/// <summary>
		/// Returns the dimensions of the texture
		/// </summary>
		virtual ivec2 GetSize() const = 0;

		/// <summary>
		/// Returns combined texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		virtual vec4 GetTexelSize() const = 0;

		/// <summary>
		/// Returns color format of the texture
		/// </summary>
		virtual Formats GetFormat() const = 0;

		/// <summary>
		/// Returns resource usage type
		/// </summary>
		virtual ResourceUsages GetUsage() const = 0;
	};

	/// <summary>
	/// Interface for 2D Textures, with SRVs
	/// </summary>
	class ITexture2D : public virtual ITexture2DBase, public IShaderResource
	{ };

	/// <summary>
	/// Interface for 2D Textures, with SRVs and UAVs
	/// </summary>
	class IRWTexture2D : public virtual ITexture2D, public IUnorderedAccess
	{ };

}