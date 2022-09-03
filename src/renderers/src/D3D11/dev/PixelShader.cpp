#include <d3dcompiler.h>
#include "D3D11/dev/PixelShader.hpp"
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/Texture2D.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader(Device& dev, const PixelShaderDef& psDef) :
	ShaderBase(dev, reinterpret_cast<const ShaderDefBase&>(psDef))
{
	ComPtr<ID3DBlob> psBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(psDef.file.data(), &psBlob));
	GFX_THROW_FAILED(dev.Get()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pPS));
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
		ID3D11DeviceContext* cur = ctx.Get();
		IDynamicCollection<ID3D11SamplerState*>& ss = samplers.GetResources();
		IDynamicCollection<ID3D11ShaderResourceView*>& tex = textures.GetResources();

		cur->PSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		cur->PSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());
		cur->PSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());

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