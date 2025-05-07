#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/EffectVariant.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Resources/ResourceSet.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(Material);

Material::Material() : pEffect(nullptr)
{ }

Material::Material(ShaderVariantManager& lib, uint nameID, uint vID) :
	ShaderInstanceBase(lib, nameID, vID), pEffect(nullptr)
{
	SetVariantID(vID);
}

uint Material::GetPassCount() const { return GetEffect().GetPassCount(); }

uint Material::GetShaderCount(int pass) const { return GetEffect().GetShaderCount(pass); }

void Material::Setup(CtxBase& ctx, int pass) { GetEffect().Setup(ctx, pass, *pRes); }

const EffectVariant& Material::GetEffect() const 
{ 
	D3D_ASSERT_MSG(nameID != -1, "Cannot access null effect");

	if (pEffect == nullptr)
	{
		const uint effectID = pLib->GetLibMap().TryGetEffectID(nameID, vID);
		pEffect = &pLib->GetEffect(effectID);
	}

	return *pEffect; 
}

void Material::SetVariantID(uint vID)
{
	if (pEffect != nullptr && vID != this->vID)
	{ 
		const uint effectID = pLib->GetLibMap().TryGetEffectID(nameID, vID);
		pEffect = &pLib->GetEffect(effectID);
		this->vID = vID;
	}
}