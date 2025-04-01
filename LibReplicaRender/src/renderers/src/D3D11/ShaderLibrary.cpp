#include "pch.hpp"
#include "ShaderLibGen/ShaderRegistryMap.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Renderer.hpp"

using namespace Replica::D3D11;

ShaderLibrary::ShaderLibrary() = default;

ShaderLibrary::ShaderLibrary(Renderer& renderer, const ShaderLibDef& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), std::move(def)))
{ }

ShaderLibrary::ShaderLibrary(Renderer& renderer, ShaderLibDef&& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), std::move(def)))
{ }

ShaderLibrary::~ShaderLibrary() = default;

const StringIDMap& ShaderLibrary::GetStringMap() const { return pManager->GetStringMap(); }

Material ShaderLibrary::GetMaterial(uint effectNameID) { return Material(*pManager, effectNameID, 0); }

ComputeInstance ShaderLibrary::GetComputeInstance(uint nameID) { return ComputeInstance(*pManager, nameID, 0); }

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
