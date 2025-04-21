#pragma once
#include "CtxBase.hpp"
#include "Resources/MappedBufferHandle.hpp"

namespace Weave::D3D11
{
	/// <summary>
	/// Device context for issuing commands to an associated D3D11 device
	/// </summary>
	class CtxImm : public CtxBase
	{
	public:
		CtxImm();

		CtxImm(Device& dev, ComPtr<ID3D11DeviceContext1>&& pCtx);

		CtxImm(CtxImm&&) noexcept;

		CtxImm& operator=(CtxImm&&) noexcept;

		~CtxImm();

		/// <summary>
		/// Returns a handle for reading and or writing the given buffer, based on the 
		/// buffer's usage type.
		/// </summary>
		MappedBufferHandle GetMappedBufferHandle(IBuffer& buffer);
	};
}