#include "pch.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"

using namespace Weave;
using namespace Weave::D3D11;

VertexShaderVariant::VertexShaderVariant(Device& dev, const ShaderDefHandle& def) :
	ShaderVariant(dev, def),
	layout(dev, def.GetBinSrc().GetData(), def.GetBinSrc().GetLength(), def.GetInLayout())
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

	if (res.GetUAVs().GetLength() > 0)
		ctx.SetUAVs(res.GetUAVs(), uavMap);
}

void ComputeShaderVariant::UnmapResources(Context& ctx) const
{
	ShaderVariant::UnmapResources(ctx);

	if (uavMap.GetCount() > 0)
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
