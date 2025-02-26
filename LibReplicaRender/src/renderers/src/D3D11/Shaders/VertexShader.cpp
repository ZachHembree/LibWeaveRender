#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Shaders/VertexShader.hpp"

using namespace Replica::D3D11;

VertexShader::VertexShader() = default;

VertexShader::VertexShader(const VertexShaderVariant& vs) :
	ShaderInstance(vs)
{
	const ShaderDef& vsDef = vs.GetDefinition();
	this->layout = InputLayout(vs.GetDevice(), vsDef.binSrc.GetPtr(), vsDef.binSrc.GetLength(), vsDef.inLayout);
}

const InputLayout& VertexShader::GetLayout() const { return layout; }

void VertexShader::Bind(Context& ctx)
{
	if (!ctx.GetIsBound(this))
	{
		ctx.SetSamplers(samplers.GetResources(), ShadeStages::Vertex);
		ctx.SetSRVs(textures.GetResources(), ShadeStages::Vertex);
		ctx.SetConstants(cBuffers, ShadeStages::Vertex);
		layout.Bind(ctx);
		
		ctx.BindShader(*this);
	}

	if (ctx.GetIsBound(this))
	{
		UpdateConstants(ctx);
	}
}

void VertexShader::Unbind(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(ShadeStages::Vertex);
	}
}

void VertexShader::UnmapResources(Context& ctx)
{
	if (ctx.GetIsBound(this))
	{
		ctx.ClearSamplers(0, (int)samplers.GetCount(), ShadeStages::Vertex);
		ctx.ClearSRVs(0, (int)textures.GetCount(), ShadeStages::Vertex);
		ctx.ClearConstants(0, (int)cBuffers.GetLength(), ShadeStages::Vertex);
	}	
}
