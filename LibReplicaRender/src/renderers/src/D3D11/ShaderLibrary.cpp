#include "pch.hpp"
#include "ReplicaEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Renderer.hpp"

using namespace Replica::D3D11;

ShaderLibrary::ShaderLibrary() = default;

ShaderLibrary::ShaderLibrary(Renderer& renderer, const ShaderLibDef& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), def))
{ }

ShaderLibrary::ShaderLibrary(Renderer& renderer, ShaderLibDef&& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), std::move(def)))
{ }

ShaderLibrary::~ShaderLibrary() = default;

const StringIDMap& ShaderLibrary::GetStringMap() const { return pManager->GetStringMap(); }

Material ShaderLibrary::GetMaterial(uint effectNameID) 
{ 
	const uint vID = pManager->GetLibMap().TryGetDefaultEffectVariant(effectNameID);
	return Material(*pManager, effectNameID, vID);
}

ComputeInstance ShaderLibrary::GetComputeInstance(uint nameID) 
{
	const uint vID = pManager->GetLibMap().TryGetDefaultShaderVariant(nameID);
	return ComputeInstance(*pManager, nameID, vID);
}

Material ShaderLibrary::GetMaterial(string_view effectName) 
{ 
	uint id = -1;

	if (GetStringMap().TryGetStringID(effectName, id))
		return GetMaterial(id);
	else
		GFX_THROW("Invalid effect name")
}

ComputeInstance ShaderLibrary::GetComputeInstance(string_view name) 
{
	uint id = -1;

	if (GetStringMap().TryGetStringID(name, id))
		return GetComputeInstance(id);
	else
		GFX_THROW("Invalid shader name")
}
