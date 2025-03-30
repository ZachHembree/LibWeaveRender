#include "pch.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"

using namespace Replica;
using namespace Replica::D3D11;

VertexShaderVariant::VertexShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def),
	layout(dev, def.GetBinSrc().GetPtr(), def.GetBinSrc().GetLength(), def.GetInLayout())
{ }

void VertexShaderVariant::Bind(Context & ctx, const ResourceSet & res) const
{
	if (!ctx.GetIsBound(this))
	{
		ctx.BindShader(*this, res);
	}
}

void VertexShaderVariant::Unbind(Context& ctx) const
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(Stage);
	}
}

void VertexShaderVariant::MapResources(Context& ctx, const ResourceSet& res) const
{
	ShaderVariant::MapResources(ctx, res);
	layout.Bind(ctx);
}

ComputeShaderVariant::ComputeShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def),
	uavMap(def.GetResources(), ShaderTypes::RandomWrite)
{}

void ComputeShaderVariant::Bind(Context & ctx, const ResourceSet & res) const
{
	if (!ctx.GetIsBound(this))
	{
		ctx.BindShader(*this, res);
	}
}

void ComputeShaderVariant::Unbind(Context& ctx) const
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(Stage);
	}
}

void ComputeShaderVariant::MapResources(Context& ctx, const ResourceSet& res) const
{
	ShaderVariant::MapResources(ctx, res);
	ctx.SetUAVs(res.GetUAVs(), uavMap);
}

void ComputeShaderVariant::UnmapResources(Context& ctx) const
{
	ShaderVariant::UnmapResources(ctx);
	ctx.ClearUAVs(0, (uint)uavMap.GetCount());
}

void PixelShaderVariant::Bind(Context& ctx, const ResourceSet& res) const
{
	if (!ctx.GetIsBound(this))
	{
		ctx.BindShader(*this, res);
	}
}

void PixelShaderVariant::Unbind(Context& ctx) const
{
	if (ctx.GetIsBound(this))
	{
		ctx.UnbindShader(Stage);
	}
}
