#include "pch.hpp"
#include <d3dcompiler.h>
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

PixelShader::PixelShader()
{ }

PixelShader::PixelShader(Device& dev, const ShaderDef& psDef) :
	ShaderBase(dev, psDef)
{
	GFX_THROW_FAILED(dev->CreatePixelShader(psDef.binSrc.GetPtr(), psDef.binSrc.GetLength(), nullptr, &pPS));
}

ID3D11PixelShader* PixelShader::Get() const
{
	return pPS.Get();
}

void PixelShader::Bind(Context& ctx)
{
	if (!ctx.GetIsPsBound(this))
	{ 
		const auto& ss = samplers.GetResources();
		const auto& tex = textures.GetResources();

		ctx->PSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		ctx->PSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());

		ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT](nullptr);

		for (int i = 0; i < cBuffers.GetLength(); i++)
		{
			pCBs[i] = cBuffers[i].Get();
		}

		ctx->PSSetConstantBuffers(0u, (uint)cBuffers.GetLength(), reinterpret_cast<ID3D11Buffer**>(&pCBs));

		this->pCtx = &ctx;
		ctx.SetPS(this);
		isBound = true;
	}

	if (ctx.GetIsPsBound(this))
	{
		int i = 0;

		for (const auto& pair : constants)
		{
			pair.second.UpdateConstantBuffer(cBuffers[i], ctx);
			i++;
		}
	}
}

void PixelShader::Unbind()
{
	if (isBound && pCtx->GetIsPsBound(this))
	{ 
		isBound = false;
		pCtx->SetPS(nullptr);
		ID3D11DeviceContext& ctx = pCtx->Get();

		ID3D11SamplerState* nullSamp[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT](nullptr);
		ctx.PSSetSamplers(0, samplers.GetCount(), nullSamp);

		ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT](nullptr);
		ctx.PSSetShaderResources(0, textures.GetCount(), nullSRV);

		ID3D11Buffer* nullCB[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT](nullptr);
		ctx.PSSetConstantBuffers(0, 1, nullCB);

		pCtx = nullptr;
	}	
}