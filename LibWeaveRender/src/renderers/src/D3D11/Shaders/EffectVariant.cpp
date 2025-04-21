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
	for (uint i = 0; i < def.GetPassCount(); i++)
	{
		const IDynamicArray<uint>& pass = def.GetPass(i);
		passes[i] = PassHandle(pass.GetLength());

		for (int j = 0; j < pass.GetLength(); j++)
		{
			const ShaderDefHandle shaderDef = effectDef.GetShader(i, j);
			const uint shaderID = pass[j];

			switch (shaderDef.GetStage())
			{
				case ShadeStages::Vertex:
					passes[i][j] = &lib.GetShader<VertexShaderVariant>(shaderID);
					break;
				case ShadeStages::Pixel:
					passes[i][j] = &lib.GetShader<PixelShaderVariant>(shaderID);
					break;
				case ShadeStages::Compute:
					passes[i][j] = &lib.GetShader<ComputeShaderVariant>(shaderID);
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
/// <returns></returns>
uint EffectVariant::GetPassCount() const { return (uint)def.GetPassCount(); }

/// <summary>
/// Returns the number of shaders in the given pass
/// </summary>
uint EffectVariant::GetShaderCount(const int pass) const { return (uint)def.GetShaderCount(pass); }

void EffectVariant::Setup(CtxBase& ctx, int pass, const ResourceSet& res) const
{
	for (int i = 0; i < passes[pass].GetLength(); i++)
	{
		const ShaderVariantBase& shader = *passes[pass][i];
		ctx.BindShader(shader, res);
	}
}

void EffectVariant::Reset(CtxBase& ctx, int pass) const
{
	for (int i = 0; i < passes[pass].GetLength(); i++)
	{
		const ShaderVariantBase& shader = *passes[pass][i];
		ctx.UnbindShader(shader);
	}
}
