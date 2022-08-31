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
	wstring_view file,
	const IDynamicCollection<IAElement>& inputDef,
	const ConstantMapDef& cDef
) :
	ShaderBase(dev, cDef)
{
	ComPtr<ID3DBlob> vsBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(file.data(), &vsBlob));
	GFX_THROW_FAILED(dev.Get()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &pVS));

	this->layout = InputLayout(dev, vsBlob, inputDef);
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

void VertexShader::SetSampler(Sampler& samp)
{
	pCtx->Get()->VSSetSamplers(0u, 1u, samp.GetAddressOf());
}

void VertexShader::SetTexture(Texture2D& tex)
{
	pCtx->Get()->VSSetShaderResources(0u, 1u, tex.GetSRVAddress());
}

