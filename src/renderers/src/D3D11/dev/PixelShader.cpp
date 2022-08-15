#include <d3dcompiler.h>
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader(Device& dev, const LPCWSTR file) :
	DeviceChild(&dev)
{
	ComPtr<ID3DBlob> psBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file, &psBlob));
	GFX_THROW_FAILED(dev.Get()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
}

ID3D11PixelShader* PixelShader::Get() const
{
	return pPS.Get();
}

void PixelShader::Bind()
{
	pDev->GetContext().Get()->PSSetShader(Get(), nullptr, 0);
}