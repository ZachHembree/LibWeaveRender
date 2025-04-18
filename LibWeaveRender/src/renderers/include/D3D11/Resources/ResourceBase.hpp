#pragma once
#pragma warning(disable: 4250)

#include "WeaveUtils/Math.hpp"
#include "../InternalD3D11.hpp"
#include "DeviceChild.hpp"
#include "ResourceEnums.hpp"
#include "Formats.hpp"

namespace Weave::D3D11
{
	class ContextBase;

	class IResource
	{
	public:
		/// <summary>
		/// Returns a pointer to the resource interface
		/// </summary>
		virtual ID3D11Resource* GetResource() = 0;

		/// <summary>
		/// Returns a pointer to the resource field address
		/// </summary>
		virtual ID3D11Resource** const GetResAddress() = 0;
	};

	class ResourceBase : public IResource, public DeviceChild
	{
	public:

	protected:
		MAKE_NO_COPY(ResourceBase)

		ResourceBase();

		ResourceBase(Device& dev);

		ResourceBase(ResourceBase&& other) noexcept;

		ResourceBase& operator=(ResourceBase&& other) noexcept;
	};

	/// <summary>
	/// Interface for types that can be bound as a shader resource
	/// </summary>
	class IShaderResource : public virtual IResource
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
	/// Interface for resource types capable of storing color data in a 2D array, but
	/// without any specific resource views.
	/// </summary>
	class IColorBuffer2D
	{
	public:
		/// <summary>
		/// Returns the dimensions of the underlying buffer
		/// </summary>
		virtual ivec2 GetSize() const = 0;

		/// <summary>
		/// Returns combined texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		virtual vec4 GetTexelSize() const = 0;

		/// <summary>
		/// Returns color format of the underlying buffer
		/// </summary>
		virtual Formats GetFormat() const = 0;
	};

	/// <summary>
	/// Interface for color buffers that whose bounds can be less than that
	/// of their underlying buffers.
	/// </summary>
	class IResizableColorBuffer2D : public virtual IColorBuffer2D
	{
	public:
		/// <summary>
		/// Returns the offset set for this target in pixels
		/// </summary>
		virtual ivec2 GetRenderOffset() const = 0;

		/// <summary>
		/// Returns the size of the render area in pixels
		/// </summary>
		virtual ivec2 GetRenderSize() const = 0;

		/// <summary>
		/// Returns combined scaled (DRS) texel size and dim fp vector.
		/// XY == Texel Size; ZW == Dim
		/// </summary>
		virtual vec4 GetRenderTexelSize() const = 0;

		/// <summary>
		/// Returns the renderSize to size ratio on (0, 1].
		/// </summary>
		virtual vec2 GetRenderScale() const = 0;
	};

	/// <summary>
	/// Interface for types that can be bound as Render Targets
	/// </summary>
	class IRenderTarget : public virtual IResizableColorBuffer2D
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
		virtual void Clear(ContextBase& ctx, vec4 color = vec4(0)) = 0;
	};

	/// <summary>
	/// Interface for types that can be used for unordered access
	/// </summary>
	class IUnorderedAccess : public virtual IResource
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
		/// Returns the range of normalized depth values accepted by the buffer
		/// </summary>
		virtual vec2 GetRange() const = 0;

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
			ContextBase& ctx,
			DSClearFlags clearFlags = DSClearFlags::Depth,
			float depthClear = 1.0f,
			UINT8 stencilClear = 0
		) = 0;
	};

	/// <summary>
	/// Interface for 2D Textures, without resource views
	/// </summary>
	class ITexture2DBase : public virtual IResource, public virtual IColorBuffer2D
	{
	public:
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
	/// Interface for resizeable 2D Textures
	/// </summary>
	class IResizeableTexture2D : public virtual ITexture2D, public virtual IResizableColorBuffer2D
	{ };

	/// <summary>
	/// Interface for 2D Textures, with SRVs and UAVs
	/// </summary>
	class IRWTexture2D : public virtual IResizeableTexture2D, public virtual IRenderTarget, public IUnorderedAccess
	{ };
}