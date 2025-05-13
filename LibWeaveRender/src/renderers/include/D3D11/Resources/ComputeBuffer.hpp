#pragma once
#include "BufferBase.hpp"

namespace Weave::D3D11
{
	class ComputeBuffer : public BufferBase, public IUnorderedAccess, public IShaderResource
	{
	public:
		DECL_DEST_MOVE(ComputeBuffer);

		ComputeBuffer();

		ComputeBuffer(Device& device, const uint count, const uint typeSize, const void* data = nullptr);

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* GetSRV() const;

		/// <summary>
		/// Returns interface to resource view
		/// </summary>
		ID3D11ShaderResourceView* const* GetSRVAddress() const;

		/// <summary>
		/// Returns pointer to UAV interface
		/// </summary>
		ID3D11UnorderedAccessView* GetUAV() override;

		/// <summary>
		/// Returns pointer to UAV pointer field
		/// </summary>
		ID3D11UnorderedAccessView* const* GetAddressUAV() override;

	private:
		UniqueComPtr<ID3D11ShaderResourceView> pSRV;
		UniqueComPtr<ID3D11UnorderedAccessView> pUAV;
	};
}