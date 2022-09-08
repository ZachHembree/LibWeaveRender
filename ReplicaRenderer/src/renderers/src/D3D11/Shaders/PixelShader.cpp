#include <d3dcompiler.h>
#include "D3D11/Shaders/PixelShader.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader()
{ }

PixelShader::PixelShader(Device& dev, const PixelShaderDef& psDef) :
	ShaderBase(dev, reinterpret_cast<const ShaderDefBase&>(psDef))
{
	ComPtr<ID3DBlob> psBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(psDef.file.data(), &psBlob));
	GFX_THROW_FAILED(dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
}

ID3D11PixelShader* PixelShader::Get() const
{
	return pPS.Get();
}

void PixelShader::SetSampler(wstring_view name, Sampler& samp)
{
	samplers.SetResource(name, samp.Get());
}

void PixelShader::SetTexture(wstring_view name, Texture2D& tex)
{
	textures.SetResource(name, tex.GetSRV());
}

void PixelShader::Bind(Context& ctx)
{
	if (!ctx.GetIsPsBound(this))
	{ 
		const auto& ss = samplers.GetResources();
		const auto& tex = textures.GetResources();

		ctx->PSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		ctx->PSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());
		ctx->PSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());

		this->pCtx = &ctx;
		ctx.SetPS(this);
		isBound = true;
	}

	if (ctx.GetIsPsBound(this))
	{
		constants.UpdateConstantBuffer(cBuf, ctx);
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