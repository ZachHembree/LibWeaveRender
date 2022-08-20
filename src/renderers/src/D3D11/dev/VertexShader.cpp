#include <d3dcompiler.h>
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/VertexShader.hpp"
#include "D3D11/dev/Texture2D.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader(
	Device& dev, 
	WSTR file, 
	const std::initializer_list<IAElement>& inputDef,
	const ConstantMapDef& cDef
) :
	ShaderBase(dev, cDef)
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file, &vsBlob));
	GFX_THROW_FAILED(dev.Get()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, inputDef);
}

ID3D11VertexShader* VertexShader::Get() const { return pVS.Get(); }

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::Bind(Context& ctx)
{
	if (!ctx.GetIsVsBound(this))
	{
		ctx.SetVS(this);
		this->pCtx = &ctx;

		constants.UpdateConstantBuffer(cBuf, ctx);
		ctx.Get()->VSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());
		layout.Bind(ctx);

		isBound = true;
	}
}

void VertexShader::Unbind()
{
	if (isBound && pCtx->GetIsVsBound(this))
	{
		pCtx->SetPS(nullptr);	
		isBound = false;
		pCtx = nullptr;
	}	
}

void VertexShader::SetConstants(ConstantBuffer& cb)
{
	assert(isBound);
	pCtx->Get()->VSSetConstantBuffers(0u, 1, cb.GetAddressOf());
}

void VertexShader::SetSampler(Sampler& samp)
{
	assert(isBound);
	pCtx->Get()->VSSetSamplers(0u, 1u, samp.GetAddressOf());
}

void VertexShader::SetTexture(Texture2D& tex)
{
	assert(isBound);
	pCtx->Get()->VSSetShaderResources(0u, 1u, tex.GetSRVAddress());
}

