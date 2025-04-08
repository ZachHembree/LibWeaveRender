#include "pch.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Shaders/ShaderInstanceBase.hpp"

using namespace Replica;
using namespace Replica::D3D11;

ShaderInstanceBase::ShaderInstanceBase() :
	pLib(nullptr), vID(-1), nameID(-1)
{ }

ShaderInstanceBase::~ShaderInstanceBase() = default;

ShaderInstanceBase::ShaderInstanceBase(ShaderVariantManager& lib, uint nameID, uint vID) :
	pLib(&lib), vID(vID), nameID(nameID), pRes(new ResourceSet())
{ }

ShaderInstanceBase::ShaderInstanceBase(ShaderInstanceBase&&) noexcept = default;

ShaderInstanceBase& ShaderInstanceBase::operator=(ShaderInstanceBase&&) noexcept = default;

uint ShaderInstanceBase::GetNameID() const { return nameID; }

string_view ShaderInstanceBase::GetName() const { return pLib->GetStringMap().GetString(nameID); }

uint ShaderInstanceBase::GetVariantID() const { return vID; }

void ShaderInstanceBase::SetConstant(uint nameID, const byte* pSrc, const size_t size) { pRes->SetConstant(nameID, pSrc, (uint)size); }

void ShaderInstanceBase::SetConstant(string_view name, const byte* pSrc, const size_t size) { pRes->SetConstant(GetStringID(name), pSrc, (uint)size); }

void ShaderInstanceBase::SetSampler(uint stringID, Sampler& samp) { pRes->SetSampler(stringID, samp.Get()); }

void ShaderInstanceBase::SetSampler(string_view name, Sampler& samp) { SetSampler(GetStringID(name), samp); }

void ShaderInstanceBase::SetTexture(uint stringID, IShaderResource& tex) { pRes->SetSRV(stringID, tex.GetSRV()); }

void ShaderInstanceBase::SetTexture(string_view name, IShaderResource& tex) { SetTexture(GetStringID(name), tex); }

void ShaderInstanceBase::SetRWTexture(uint stringID, IUnorderedAccess& rwTex) { pRes->SetUAV(stringID, rwTex.GetUAV()); }

void ShaderInstanceBase::SetRWTexture(string_view name, IUnorderedAccess& rwTex) { SetRWTexture(GetStringID(name), rwTex); }

uint ShaderInstanceBase::GetStringID(string_view str) const
{
	uint id = -1;

	if (TryGetStringID(str, id))
		return id;
	else
		GFX_THROW("Cannot retrieve ID for unregistered string")
}

bool ShaderInstanceBase::TryGetStringID(string_view str, uint& id) const
{
	GFX_ASSERT(pLib != nullptr, "Cannot retrieve stringID from invalid shader library")
	return pLib->GetStringMap().TryGetStringID(str, id);
}

string_view ShaderInstanceBase::GetString(uint stringID) const { return pLib->GetLibMap().GetStringMap().GetString(stringID); }

void ShaderInstanceBase::GetDefines(Vector<string_view>& names) const { pLib->GetLibMap().GetDefines(vID, names); }

bool ShaderInstanceBase::GetIsDefined(uint nameID) const { return pLib->GetLibMap().GetIsDefined(nameID, vID); }

bool ShaderInstanceBase::GetIsDefined(string_view name) const { return pLib->GetLibMap().GetIsDefined(name, vID); }

void ShaderInstanceBase::SetFlag(uint nameID, bool value) 
{
	const ShaderLibMap& map = pLib->GetLibMap();
	const uint flag = map.TryGetFlag(nameID, vID);
	SetVariantID(map.SetFlags(flag, value, vID));
}

void ShaderInstanceBase::SetFlag(string_view name, bool value) { SetVariantID(pLib->GetLibMap().SetFlag(name, value, vID)); }

void ShaderInstanceBase::SetMode(uint nameID) 
{ 
	const ShaderLibMap& map = pLib->GetLibMap();
	const uint mode = map.TryGetModeID(nameID, vID);
	SetVariantID(pLib->GetLibMap().SetMode(mode, vID)); 
}

void ShaderInstanceBase::SetMode(string_view name) { SetVariantID(pLib->GetLibMap().SetMode(name, vID)); }

void ShaderInstanceBase::ResetMode() { SetVariantID(pLib->GetLibMap().ResetMode(vID)); }

void ShaderInstanceBase::ResetDefines() { SetVariantID(pLib->GetLibMap().ResetVariant(0)); }

void ShaderInstanceBase::GetDefines(Vector<uint>& nameIDs) const { pLib->GetLibMap().GetDefines(vID, nameIDs); }
