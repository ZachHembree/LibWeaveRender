#include <d3dcompiler.h>
#include "D3D11/Device.hpp"
#include "D3D11/Shaders/VertexShader.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader()
{ }

VertexShader::VertexShader(
	Device& dev, 
	const VertexShaderDef& vsDef
) :
	ShaderBase(dev, reinterpret_cast<const ShaderDefBase&>(vsDef))
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(vsDef.file.data(), &vsBlob));
	GFX_THROW_FAILED(dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, vsDef.iaLayout);
}

ID3D11VertexShader* VertexShader::Get() const { return pVS.Get(); }

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::SetSampler(wstring_view name, Sampler& samp)
{
	samplers.SetResource(name, samp.Get());
}

void VertexShader::SetTexture(wstring_view name, Texture2D& tex)
{
	textures.SetResource(name, tex.GetSRV());
}

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
		ctx->VSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());
		layout.Bind(ctx);

		isBound = true;
	}

	if (ctx.GetIsVsBound(this))
	{
		constants.UpdateConstantBuffer(cBuf, ctx);
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
