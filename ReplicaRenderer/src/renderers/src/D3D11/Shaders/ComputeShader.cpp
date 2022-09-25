#include <d3dcompiler.h>
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;
using namespace Microsoft::WRL;

ComputeShader::ComputeShader() 
{ }

ComputeShader::ComputeShader(Device& dev, const ComputeShaderDef& csDef) :
	ShaderBase(dev, csDef),
	uavBuffers(csDef.uavBuffers)
{
	ComPtr<ID3DBlob> csBlob;
	GFX_THROW_FAILED(D3DReadFileToBlob(csDef.file.data(), &csBlob));
	GFX_THROW_FAILED(dev->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &pCS));
}

ID3D11ComputeShader* ComputeShader::Get() const
{
	return pCS.Get();
}

void ComputeShader::SetRWTexture(wstring_view name, IRWTexture2D& tex)
{
	uavBuffers.SetResource(name, tex.GetUAV());
}

void ComputeShader::Dispatch(Context& ctx, int groups)
{
	Dispatch(ctx, ivec3(groups, 1, 1));
}

void ComputeShader::Dispatch(Context& ctx, ivec2 groups)
{
	Dispatch(ctx, ivec3(groups, 1));
}

void ComputeShader::Dispatch(Context& ctx, ivec3 groups)
{
	Bind(ctx);
	ctx->Dispatch(groups.x, groups.y, groups.z);
	Unbind();
}

void ComputeShader::Bind(Context& ctx)
{
	if (!ctx.GetIsCsBound(this))
	{
		ctx.SetCS(this);
		this->pCtx = &ctx;

		const auto& ss = samplers.GetResources();
		const auto& tex = textures.GetResources();
		const auto& uav = uavBuffers.GetResources();

		ctx->CSSetSamplers(0, (UINT)ss.GetLength(), ss.GetPtr());
		ctx->CSSetShaderResources(0, (UINT)tex.GetLength(), tex.GetPtr());
		ctx->CSSetConstantBuffers(0u, 1, cBuf.GetAddressOf());
		ctx->CSSetUnorderedAccessViews(0u, (UINT)uav.GetLength(), uav.GetPtr(), 0);

		isBound = true;
	}

	if (ctx.GetIsCsBound(this))
	{
		constants.UpdateConstantBuffer(cBuf, ctx);
	}
}

void ComputeShader::Unbind()
{
	if (isBound && pCtx->GetIsCsBound(this))
	{
		isBound = false;
		pCtx->SetPS(nullptr);
		ID3D11DeviceContext& ctx = pCtx->Get();

		ID3D11SamplerState* nullSamp[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT](nullptr);
		ctx.CSSetSamplers(0, samplers.GetCount(), nullSamp);

		ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT](nullptr);
		ctx.CSSetShaderResources(0, textures.GetCount(), nullSRV);

		ID3D11Buffer* nullCB[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT](nullptr);
		ctx.CSSetConstantBuffers(0, 1, nullCB);

		ID3D11UnorderedAccessView* nullUAV[D3D11_1_UAV_SLOT_COUNT](nullptr);
		ctx.CSSetUnorderedAccessViews(0, uavBuffers.GetCount(), nullUAV, 0);

		pCtx = nullptr;
	}
}