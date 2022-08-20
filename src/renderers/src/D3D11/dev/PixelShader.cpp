#include <d3dcompiler.h>
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/Texture2D.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader(Device& dev, const LPCWSTR file) :
	ShaderBase(dev)
{
	ComPtr<ID3DBlob> psBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file, &psBlob));
	GFX_THROW_FAILED(dev.Get()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
}

ID3D11PixelShader* PixelShader::Get() const
{
	return pPS.Get();
}

void PixelShader::SetConstants(ConstantBuffer& cb)
{
	assert(isBound);
	pCtx->Get()->PSSetConstantBuffers(0u, 1, cb.GetAddressOf());
}

void PixelShader::SetSampler(Sampler& samp)
{
	assert(isBound);
	pCtx->Get()->PSSetSamplers(0u, 1u, samp.GetAddressOf());
}

void PixelShader::SetTexture(Texture2D& tex)
{
	assert(isBound);
	pCtx->Get()->PSSetShaderResources(0u, 1u, tex.GetSRVAddress());
}

void PixelShader::Bind(Context& ctx)
{
	if (!ctx.GetIsPsBound(this))
	{ 
		this->pCtx = &ctx;
		ctx.SetPS(this);
		isBound = true;
	}
}

void PixelShader::Unbind()
{
	if (isBound && pCtx->GetIsPsBound(this))
	{ 
		pCtx->SetPS(nullptr);
		isBound = false;
		pCtx = nullptr;
	}	
}