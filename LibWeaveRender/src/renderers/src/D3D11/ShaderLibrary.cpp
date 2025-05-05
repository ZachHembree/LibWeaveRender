#include "pch.hpp"
#include "WeaveEffects/ShaderLibBuilder/ShaderRegistryMap.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ShaderLibrary.hpp"
#include "D3D11/Shaders/Material.hpp"
#include "D3D11/Shaders/ComputeInstance.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Renderer.hpp"

using namespace Weave::D3D11;

ShaderLibrary::ShaderLibrary() = default;

ShaderLibrary::ShaderLibrary(Renderer& renderer, const ShaderLibDef& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), def))
{ }

ShaderLibrary::ShaderLibrary(Renderer& renderer, ShaderLibDef&& def) :
	pManager(new ShaderVariantManager(renderer.GetDevice(), std::move(def)))
{ }

inline Weave::D3D11::ShaderLibrary::ShaderLibrary(ShaderLibrary&&) noexcept = default;

ShaderLibrary& ShaderLibrary::operator=(ShaderLibrary&&) noexcept = default;

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
	D3D_CHECK_MSG(GetStringMap().TryGetStringID(effectName, id), "Invalid effect name");
	return GetMaterial(id);
}

ComputeInstance ShaderLibrary::GetComputeInstance(string_view name) 
{
	uint id = -1;
	D3D_CHECK_MSG(GetStringMap().TryGetStringID(name, id), "Invalid shader name");
	return GetComputeInstance(id);
}
