#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Shaders/ComputeShader.hpp"

using namespace Replica::D3D11;

ComputeShader::ComputeShader() = default;

ComputeShader::ComputeShader(const ComputeShaderVariant& cs) :
	ShaderInstance(cs),
	uavBuffers(cs.GetDefinition().res, ShaderTypes::RandomWrite)
{ }

void ComputeShader::SetRWTexture(string_view name, IRWTexture2D& tex)
{
	uavBuffers.SetResource(name, tex.GetUAV());
}

void ComputeShader::Dispatch(Context& ctx, int groups) { Dispatch(ctx, ivec3(groups, 1, 1)); }

void ComputeShader::Dispatch(Context& ctx, ivec2 groups) { Dispatch(ctx, ivec3(groups, 1)); }

void ComputeShader::Dispatch(Context& ctx, ivec3 groups) { ctx.Dispatch(*this, groups); }

void ComputeShader::Bind(Context& ctx)
{
	if (!ctx.GetIsBound(this))
	{
		ctx.SetSamplers(samplers.GetResources(), ShadeStages::Compute);
		ctx.SetSRVs(textures.GetResources(), ShadeStages::Compute);
		ctx.SetConstants(cBuffers, ShadeStages::Compute);

		ctx.SetUAVs(uavBuffers.GetResources());
		ctx.BindShader(*this);
	}

	if (ctx.GetIsBound(this))
	{
		UpdateConstants(ctx);
	}
}

void ComputeShader::Unbind(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(ShadeStages::Compute);
	}
}

void ComputeShader::UnmapResources(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.ClearSamplers(0, (int)samplers.GetCount(), ShadeStages::Compute);
		ctx.ClearSRVs(0, (int)textures.GetCount(), ShadeStages::Compute);
		ctx.ClearConstants(0, (int)cBuffers.GetLength(), ShadeStages::Compute);
		ctx.ClearUAVs(0, (int)uavBuffers.GetCount());
	}
}