#include "pch.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/Resources/ResourceSet.hpp"

using namespace Replica;
using namespace Replica::D3D11;

void ComputeInstance::Dispatch(Context & ctx, ivec3 groups) { ctx.Dispatch(GetShader(), groups, *pRes); }

void  ComputeInstance::Dispatch(Context& ctx, ivec2 groups) { Dispatch(ctx, ivec3(groups.x, groups.y, 1)); }

void  ComputeInstance::Dispatch(Context& ctx, int groups) { Dispatch(ctx, ivec3(groups, 1, 1)); }

const ComputeShaderVariant& ComputeInstance::GetShader() const 
{ 
	GFX_ASSERT(nameID != -1, "Cannot retrieve null shader")

	if (pCS == nullptr)
	{
		const uint shaderID = pLib->GetLibMap().TryGetShaderID(nameID, vID);
		pCS = &pLib->GetShader<ComputeShaderVariant>(shaderID);
	}

	return *pCS; 
}

void ComputeInstance::SetVariantID(int vID)
{
	if (vID != this->vID)
	{ 
		const uint effectID = pLib->GetLibMap().TryGetShaderID(nameID, vID);
		pCS = &pLib->GetShader<ComputeShaderVariant>(effectID);
		this->vID = vID;
	}
}

