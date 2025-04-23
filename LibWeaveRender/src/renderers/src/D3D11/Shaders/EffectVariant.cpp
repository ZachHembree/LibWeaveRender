#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Shaders/EffectVariant.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"

using namespace Weave;
using namespace Weave::D3D11;

EffectVariant::EffectVariant() = default;

EffectVariant::EffectVariant(ShaderVariantManager& lib, EffectDefHandle effectDef) :
	def(effectDef),
	passes(effectDef.GetPassCount())
{ 
	for (uint passDef = 0; passDef < def.GetPassCount(); passDef++)
	{
		const IDynamicArray<uint>& pass = def.GetPass(passDef);
		passes[passDef].shaders = DynamicArray<const ShaderVariantBase*>(pass.GetLength());
		passes[passDef].activeStages = {};

		for (uint shader = 0; shader < pass.GetLength(); shader++)
		{
			const ShaderDefHandle shaderDef = effectDef.GetShader(passDef, shader);
			const uint shaderID = pass[shader];
			const ShadeStages stage = shaderDef.GetStage();

			switch (shaderDef.GetStage())
			{
				case ShadeStages::Vertex:
					passes[passDef].shaders[shader] = &lib.GetShader<VertexShaderVariant>(shaderID);
					passes[passDef].activeStages[(uint)stage] = true;
					break;
				case ShadeStages::Pixel:
					passes[passDef].shaders[shader] = &lib.GetShader<PixelShaderVariant>(shaderID);
					passes[passDef].activeStages[(uint)stage] = true;
					break;
				case ShadeStages::Compute:
					passes[passDef].shaders[shader] = &lib.GetShader<ComputeShaderVariant>(shaderID);
					passes[passDef].activeStages[(uint)stage] = true;
					break;
				default:
					D3D_ASSERT_MSG("Unsupported effect shader specified");
					break;
			}
		}
	}
}

/// <summary>
/// Returns the name of the effect
/// </summary>
uint EffectVariant::GetNameID() const { return def.GetNameID(); }

/// <summary>
/// Returns the number of passes in the effect
/// </summary>
uint EffectVariant::GetPassCount() const { return (uint)def.GetPassCount(); }

/// <summary>
/// Returns the number of shaders in the given pass
/// </summary>
uint EffectVariant::GetShaderCount(const int pass) const { return (uint)def.GetShaderCount(pass); }

void EffectVariant::Setup(CtxBase& ctx, int pass, const ResourceSet& res) const
{
	ctx.SetActiveStages(passes[pass].activeStages);

	for (const ShaderVariantBase* pShader : passes[pass].shaders)
	{
		ctx.BindShader(*pShader, res);
	}
}
