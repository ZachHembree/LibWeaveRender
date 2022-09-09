#include <d3dcompiler.h>
#include "D3D11/Device.hpp"
#include "D3D11/Resources/RWTexture2D.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Shaders/ComputeShader.hpp"

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

void ComputeShader::SetRWTexture(wstring_view name, RWTexture2D& tex)
{
	uavBuffers.SetResource(name, tex.GetUAV());
}

void ComputeShader::Dispatch(Context& ctx, ivec3 groups)
{
	Bind(ctx);
	ctx->Dispatch(groups.x, groups.y, groups.z);
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
		pCtx->SetPS(nullptr);
		isBound = false;
		pCtx = nullptr;
	}
}