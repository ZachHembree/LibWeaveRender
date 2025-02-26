#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Shaders/PixelShader.hpp"

using namespace Replica::D3D11;

PixelShader::PixelShader() = default;

PixelShader::PixelShader(const PixelShaderVariant& ps) :
	ShaderInstance(ps)
{ }

void PixelShader::Bind(Context& ctx)
{
	if (!ctx.GetIsBound(this))
	{ 
		ctx.SetSamplers(samplers.GetResources(), ShadeStages::Pixel);
		ctx.SetSRVs(textures.GetResources(), ShadeStages::Pixel);
		ctx.SetConstants(cBuffers, ShadeStages::Pixel);

		ctx.BindShader(*this);
	}

	if (ctx.GetIsBound(this))
	{
		UpdateConstants(ctx);
	}
}

void PixelShader::Unbind(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(ShadeStages::Pixel);
	}
}

void PixelShader::UnmapResources(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.ClearSamplers(0, (int)samplers.GetCount(), ShadeStages::Pixel);
		ctx.ClearSRVs(0, (int)textures.GetCount(), ShadeStages::Pixel);
		ctx.ClearConstants(0, (int)cBuffers.GetLength(), ShadeStages::Pixel);
	}
}