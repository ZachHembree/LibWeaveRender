#include <d3dcompiler.h>
#include "D3D11/Device.hpp"
#include "D3D11/Shaders/VertexShader.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/Sampler.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

VertexShader::VertexShader(
	Device& dev, 
	const VertexShaderDef& vsDef
) :
	ShaderBase(dev, reinterpret_cast<const ShaderDefBase&>(vsDef))
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(vsDef.file.data(), &vsBlob));
	GFX_THROW_FAILED(dev.Get()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, vsDef.iaLayout);
}

VertexShader::VertexShader(VertexShader&& other) noexcept :
	ShaderBase(std::move(other)),
	layout(std::move(other.layout)),
	pVS(std::move(other.pVS))
{ }

VertexShader& VertexShader::operator=(VertexShader&& other) noexcept
{
	ShaderBase::operator=(std::move(other));
	this->layout = std::move(other.layout);
	this->pVS = std::move(other.pVS);

	return *this;
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

		ID3D11DeviceContext* cur = ctx.Get();
		IDynamicCollection<ID3D11SamplerState*>& ss = samplers.GetResources();
		IDynamicCollection<ID3D11ShaderResourceView*>& tex = textures.GetResources();

		cur->VSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		cur->VSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());		
		cur->VSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());
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
