#include "pch.hpp"
#include <d3dcompiler.h>
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader()
{ }

VertexShader::VertexShader(
	Device& dev, 
	const ShaderDef& vsDef
) :
	ShaderBase(dev, vsDef)
{
	GFX_THROW_FAILED(dev->CreateVertexShader(vsDef.binSrc.GetPtr(), vsDef.binSrc.GetLength(), nullptr, &pVS));
	this->layout = InputLayout(dev, vsDef.binSrc.GetPtr(), vsDef.binSrc.GetLength(), vsDef.inLayout);
}

ID3D11VertexShader* VertexShader::Get() const { return pVS.Get(); }

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::Bind(Context& ctx)
{
	if (!ctx.GetIsVsBound(this))
	{
		ctx.SetVS(this);
		this->pCtx = &ctx;

		const auto& ss = samplers.GetResources();
		const auto& tex = textures.GetResources();

		ctx->VSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		ctx->VSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());		

		ID3D11Buffer* pCBs[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT](nullptr);

		for (int i = 0; i < cBuffers.GetLength(); i++)
		{
			pCBs[i] = cBuffers[i].Get();
		}

		ctx->VSSetConstantBuffers(0u, (uint)cBuffers.GetLength(), reinterpret_cast<ID3D11Buffer**>(&pCBs));
		layout.Bind(ctx);

		isBound = true;
	}

	if (ctx.GetIsVsBound(this))
	{
		int i = 0;

		for (const auto& pair : constants)
		{
			pair.second.UpdateConstantBuffer(cBuffers[i], ctx);
			i++;
		}
	}
}

void VertexShader::Unbind()
{
	if (isBound && pCtx->GetIsVsBound(this))
	{
		isBound = false;
		pCtx->SetVS(nullptr);	
		ID3D11DeviceContext& ctx = pCtx->Get();

		ID3D11SamplerState* nullSamp[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT](nullptr);
		ctx.VSSetSamplers(0, samplers.GetCount(), nullSamp);

		ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT](nullptr);
		ctx.VSSetShaderResources(0, textures.GetCount(), nullSRV);

		ID3D11Buffer* nullCB[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT](nullptr);
		ctx.VSSetConstantBuffers(0, 1, nullCB);
	
		pCtx = nullptr;
	}	
}
