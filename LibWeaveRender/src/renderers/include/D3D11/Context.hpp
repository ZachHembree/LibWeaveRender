#pragma once
#include "ContextBase.hpp"
#include "Resources/MappedBufferHandle.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Device context for issuing commands to an associated D3D11 device
	/// </summary>
	class Context : public ContextBase
	{
	public:
		Context();

		Context(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx);

		Context(Context&&) noexcept;

		Context& operator=(Context&&) noexcept;

		~Context();

		/// <summary>
		/// Returns a handle for reading and or writing the given buffer, based on the 
		/// buffer's usage type.
		/// </summary>
		MappedBufferHandle GetMappedBufferHandle(IBuffer& buffer);
	};
}