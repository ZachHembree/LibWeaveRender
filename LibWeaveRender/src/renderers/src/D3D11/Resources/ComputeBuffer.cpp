#include "pch.hpp"
#include "D3D11/Resources/ComputeBuffer.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Device.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(ComputeBuffer);

ComputeBuffer::ComputeBuffer() = default;

ComputeBuffer::ComputeBuffer(Device& device, uint typeSize) :
	ComputeBufferBase(
		ResourceBindFlags::UnorderedAccess | ResourceBindFlags::ShaderResource,
		ResourceUsages::Default,
		ResourceAccessFlags::None,
		device,
		typeSize
	)
{
	Init();
}

ComputeBuffer::ComputeBuffer(Device& device, uint count, uint typeSize, const void* data) :
	ComputeBufferBase(
		ResourceBindFlags::UnorderedAccess | ResourceBindFlags::ShaderResource,
		ResourceUsages::Default,
		ResourceAccessFlags::None,
		device,
		typeSize,
		count,
		data
	)
{
	Init();
}

ID3D11ShaderResourceView* ComputeBuffer::GetSRV() const { return pSRV.Get(); }

ID3D11ShaderResourceView* const* ComputeBuffer::GetSRVAddress() const { return pSRV.GetAddressOf(); }

ID3D11UnorderedAccessView* ComputeBuffer::GetUAV() { return pUAV.Get(); }

ID3D11UnorderedAccessView* const* ComputeBuffer::GetAddressUAV() { return pUAV.GetAddressOf(); }

void ComputeBuffer::Init()
{
	if (pBuf != nullptr)
	{
		// SRV
		D3D11_SHADER_RESOURCE_VIEW_DESC vDesc = {};
		vDesc.Format = (DXGI_FORMAT)Formats::UNKNOWN;
		vDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		vDesc.Buffer.FirstElement = 0;
		vDesc.Buffer.NumElements = count;

		D3D_CHECK_HR(GetDevice()->CreateShaderResourceView(pBuf.Get(), &vDesc, &pSRV));

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = (DXGI_FORMAT)Formats::UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = count;

		D3D_CHECK_HR(GetDevice()->CreateUnorderedAccessView(pBuf.Get(), &uavDesc, &pUAV));
	}
}
