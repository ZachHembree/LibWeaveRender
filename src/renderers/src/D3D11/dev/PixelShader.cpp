#include <d3dcompiler.h>
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/Texture2D.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader(Device& dev, const wstring_view file, const ConstantMapDef& cDefPS) :
	ShaderBase(dev, cDefPS)
{
	ComPtr<ID3DBlob> psBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file.data(), &psBlob));
	GFX_THROW_FAILED(dev.Get()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
}

ID3D11PixelShader* PixelShader::Get() const
{
	return pPS.Get();
}

void PixelShader::SetSampler(Sampler& samp)
{
	pCtx->Get()->PSSetSamplers(0u, 1u, samp.GetAddressOf());
}

void PixelShader::SetTexture(Texture2D& tex)
{
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