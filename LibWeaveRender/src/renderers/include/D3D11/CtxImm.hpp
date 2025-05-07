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
		DECL_DEST_MOVE(CtxImm);

		CtxImm();

		CtxImm(Device& dev, UniqueComPtr<ID3D11DeviceContext1>&& pCtx);

		/// <summary>
		/// Returns a handle for reading and or writing the given buffer, based on the 
		/// buffer's usage type.
		/// </summary>
		MappedBufferHandle GetMappedBufferHandle(IBuffer& buffer);

	private:
		friend Renderer;
		void EndFrame();
	};
}