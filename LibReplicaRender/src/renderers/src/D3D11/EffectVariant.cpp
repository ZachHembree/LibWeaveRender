#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/EffectVariant.hpp"
#include "D3D11/Shaders/VertexShader.hpp"
#include "D3D11/Shaders/PixelShader.hpp"
#include "D3D11/ShaderLibrary.hpp"

using namespace Replica;
using namespace Replica::D3D11;

EffectVariant::EffectVariant() : pLib(nullptr), pDef(nullptr) {}

EffectVariant::EffectVariant(ShaderLibrary& lib, const EffectDef& effectDef) :
	pLib(&lib),
	pDef(&effectDef)
{ }

/// <summary>
/// Returns the name of the effect
/// </summary>
string_view EffectVariant::GetName() const { return pDef->name; }

/// <summary>
/// Returns the unique identifier for the effect
/// </summary>
uint EffectVariant::GetVariantID() const { return pDef->variantID; }

/// <summary>
/// Returns the library that owns the effect
/// </summary>
const ShaderLibrary& EffectVariant::GetLibrary() const { GFX_ASSERT(pLib != nullptr, "Cannot access null shader library"); return *pLib; }

/// <summary>
/// Returns the number of passes in the effect
/// </summary>
/// <returns></returns>
int EffectVariant::GetPassCount() const { return (int)pDef->passes.GetLength(); }

/// <summary>
/// Returns the number of shaders in the given pass
/// </summary>
int EffectVariant::GetShaderCount(const int pass) const { return (int)pDef->passes[pass].shaderIDs.GetLength(); }

void EffectVariant::SetSampler(string_view name, Sampler& samp)
{
	for (int i = 0; i < GetPassCount(); i++)
	{
		for (int j = 0; j < GetShaderCount(i); j++)
			GetShader(j, i).SetSampler(name, samp);
	}
}

/// <summary>
/// Assigns a Texture2D with the given name
/// </summary>
void EffectVariant::SetTexture(string_view name, ITexture2D& tex)
{
	for (int i = 0; i < GetPassCount(); i++)
	{
		for (int j = 0; j < GetShaderCount(i); j++)
			GetShader(j, i).SetTexture(name, tex);
	}
}

void EffectVariant::Setup(Context& ctx, int pass) // To-Do: multi-pass support
{
	for (int i = 0; i < GetShaderCount(pass); i++)
		GetShader(i, pass).Bind(ctx);
}

void EffectVariant::Reset(Context& ctx, int pass)
{
	for (int i = 0; i < GetShaderCount(pass); i++)
		GetShader(i, pass).Unbind(ctx);
}

ShaderBase& EffectVariant::GetShader(int index, int pass)
{
	// Allocate pass containers - lazy init
	if (passes.GetLength() != GetPassCount())
		passes = UniqueArray<EffectPass>(GetPassCount());

	const IDynamicArray<int>& shaderIDs = pDef->passes[pass].shaderIDs;

	if (passes[pass].GetLength() != shaderIDs.GetLength())
		passes[pass] = UniqueArray<ShaderHandle>(shaderIDs.GetLength());

	ShaderHandle& shader = passes[pass][index];

	// Create shader variant on demand
	if (shader.get() == nullptr)
	{
		const ShaderDef& def = pLib->GetLibMap().GetShader(shaderIDs[index], GetVariantID());

		switch (def.stage)
		{
		case ShadeStages::Vertex:
			shader.reset(new VertexShader( pLib->GetShader<VertexShaderVariant>(shaderIDs[index], GetVariantID()) ));
			break;
		case ShadeStages::Pixel:
			shader.reset(new PixelShader( pLib->GetShader<PixelShaderVariant>(shaderIDs[index], GetVariantID()) ));
			break;
		default:
			GFX_THROW("Shader type not supported by effects")
		}
	}

	return *shader.get();
}
