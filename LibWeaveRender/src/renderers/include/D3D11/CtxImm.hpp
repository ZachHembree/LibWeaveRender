#pragma once
#include <functional>
#include "CtxBase.hpp"

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

		/// <summary>
		/// Starts a non-blocking asynchronous readback of the given buffer with a callback to 
		/// be invoked when the buffer is ready.
		/// </summary>
		void BeginAsyncBufferRead(IBuffer& buffer, const ReadCallback& callbackFunc) override;

	private:
		UniqueVector<std::pair<MappedBufferHandle, ReadCallback>> readbackQueue;

		friend Renderer;
		void EndFrame();
	};
}