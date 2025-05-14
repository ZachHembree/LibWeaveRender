#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Shaders/ShaderVariants.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/Resources/ResourceSet.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(ComputeInstance);

ComputeInstance::ComputeInstance() :
	pCS(nullptr)
{ }

ComputeInstance::ComputeInstance(ShaderVariantManager& lib, uint nameID, uint vID)  : 
	ShaderInstanceBase(lib, nameID, vID), pCS(nullptr)
{ 
	SetVariantID(vID);
}

uivec3 ComputeInstance::GetThreadGroupSize() const { return GetShader().GetDefinition().GetThreadGroupSize(); }

void ComputeInstance::SetKernel(uint nameID)
{
	const uint shaderID = pLib->GetLibMap().TryGetShaderID(nameID, vID);
	pCS = &pLib->GetShader<ComputeShaderVariant>(shaderID);
	this->nameID = nameID;
}

void ComputeInstance::SetKernel(string_view name) { SetKernel(GetStringID(name)); }

ComputeBufferHandle ComputeInstance::GetComputeBuffer(uint nameID) { return pRes->GetSRV<ComputeBufferHandle>(nameID); }

ComputeBufferHandle ComputeInstance::GetComputeBuffer(string_view name) { return pRes->GetSRV<ComputeBufferHandle>(GetStringID(name)); }

void ComputeInstance::SetComputeBuffer(uint nameID, const IShaderResource& buf) { pRes->SetSRV(nameID, buf); }

void ComputeInstance::SetComputeBuffer(string_view name, const IShaderResource& buf) { SetComputeBuffer(GetStringID(name), buf); }

RWComputeBufferHandle ComputeInstance::GetRWComputeBuffer(uint nameID) { return pRes->GetUAV<RWComputeBufferHandle>(nameID); }

RWComputeBufferHandle ComputeInstance::GetRWComputeBuffer(string_view name) { return pRes->GetUAV<RWComputeBufferHandle>(GetStringID(name)); }

void ComputeInstance::SetRWComputeBuffer(uint nameID, IUnorderedAccess& buf) { pRes->SetUAV(nameID, buf); }

void ComputeInstance::SetRWComputeBuffer(string_view name, IUnorderedAccess& buf) { SetRWComputeBuffer(GetStringID(name), buf); }

void ComputeInstance::Dispatch(CtxBase& ctx, ivec3 groups) { ctx.Dispatch(GetShader(), groups, *pRes); }

void  ComputeInstance::Dispatch(CtxBase& ctx, ivec2 groups) { Dispatch(ctx, ivec3(groups.x, groups.y, 1)); }

void  ComputeInstance::Dispatch(CtxBase& ctx, int groups) { Dispatch(ctx, ivec3(groups, 1, 1)); }

const ComputeShaderVariant& ComputeInstance::GetShader() const 
{ 
	D3D_ASSERT_MSG(nameID != -1, "Cannot retrieve null shader");

	if (pCS == nullptr)
	{
		const uint shaderID = pLib->GetLibMap().TryGetShaderID(nameID, vID);
		pCS = &pLib->GetShader<ComputeShaderVariant>(shaderID);
	}

	return *pCS; 
}

void ComputeInstance::SetVariantID(uint vID)
{
	if (pCS != nullptr && vID != this->vID)
	{ 
		const uint shaderID = pLib->GetLibMap().TryGetShaderID(nameID, vID);
		pCS = &pLib->GetShader<ComputeShaderVariant>(shaderID);
		this->vID = vID;
	}
}

