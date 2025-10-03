#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/ShaderVariantManager.hpp"
#include "D3D11/Shaders/ShaderInstanceBase.hpp"

using namespace Weave;
using namespace Weave::D3D11;

DEF_DEST_MOVE(ShaderInstanceBase);

ShaderInstanceBase::ShaderInstanceBase() :
	pLib(nullptr), vID(-1), nameID(-1)
{ }

ShaderInstanceBase::ShaderInstanceBase(ShaderVariantManager& lib, uint nameID, uint vID) :
	pLib(&lib), vID(vID), nameID(nameID), pRes(new ResourceSet())
{ }

uint ShaderInstanceBase::GetNameID() const { return nameID; }

const StringSpan ShaderInstanceBase::GetName() const { return pLib->GetStringMap().GetString(nameID); }

uint ShaderInstanceBase::GetVariantID() const { return vID; }

void ShaderInstanceBase::SetConstant(uint nameID, const Span<byte>& newValue) { pRes->SetConstant(nameID, newValue); }

void ShaderInstanceBase::SetConstant(string_view name, const Span<byte>& newValue) { pRes->SetConstant(GetStringID(name), newValue); }

SamplerHandle ShaderInstanceBase::GetSampler(uint stringID) { return pRes->GetSampler(stringID); }

SamplerHandle ShaderInstanceBase::GetSampler(string_view name) { return pRes->GetSampler(GetStringID(name)); }

void ShaderInstanceBase::SetSampler(uint stringID, const Sampler& samp) { pRes->SetSampler(stringID, samp); }

void ShaderInstanceBase::SetSampler(string_view name, const Sampler& samp) { SetSampler(GetStringID(name), samp); }

TextureHandle ShaderInstanceBase::GetTexture(uint stringID) { return pRes->GetSRV<TextureHandle>(stringID); }

TextureHandle ShaderInstanceBase::GetTexture(string_view name) { return pRes->GetSRV<TextureHandle>(GetStringID(name)); }

void ShaderInstanceBase::SetTexture(uint stringID, const IShaderResource& tex) { pRes->SetSRV(stringID, tex); }

void ShaderInstanceBase::SetTexture(string_view name, const IShaderResource& tex) { SetTexture(GetStringID(name), tex); }

RWTextureHandle ShaderInstanceBase::GetRWTexture(uint stringID) { return pRes->GetUAV<RWTextureHandle>(stringID); }

RWTextureHandle ShaderInstanceBase::GetRWTexture(string_view name) { return pRes->GetUAV<RWTextureHandle>(GetStringID(name)); }

void ShaderInstanceBase::SetRWTexture(uint stringID, IUnorderedAccess& rwTex) { pRes->SetUAV(stringID, rwTex); }

void ShaderInstanceBase::SetRWTexture(string_view name, IUnorderedAccess& rwTex) { SetRWTexture(GetStringID(name), rwTex); }

uint ShaderInstanceBase::GetStringID(string_view str) const
{
	uint id = -1;
	D3D_CHECK_MSG(TryGetStringID(str, id), "Cannot retrieve ID for unregistered string");
	return id;
}

bool ShaderInstanceBase::TryGetStringID(string_view str, uint& id) const
{
	D3D_ASSERT_MSG(pLib != nullptr, "Cannot retrieve stringID from invalid shader library");
	return pLib->GetStringMap().TryGetStringID(str, id);
}

const StringSpan ShaderInstanceBase::GetString(uint stringID) const { return pLib->GetLibMap().GetStringMap().GetString(stringID); }

void ShaderInstanceBase::GetDefines(Vector<string_view>& names) const { pLib->GetLibMap().GetDefines(vID, names); }

bool ShaderInstanceBase::GetIsDefined(uint nameID) const { return pLib->GetLibMap().GetIsDefined(nameID, vID); }

bool ShaderInstanceBase::GetIsDefined(string_view name) const { return pLib->GetLibMap().GetIsDefined(name, vID); }

void ShaderInstanceBase::SetFlag(uint nameID, bool value) { SetVariantID(pLib->GetLibMap().SetFlag(nameID, value, vID)); }

void ShaderInstanceBase::SetFlag(string_view name, bool value) { SetVariantID(pLib->GetLibMap().SetFlag(name, value, vID)); }

void ShaderInstanceBase::SetMode(uint nameID) { SetVariantID(pLib->GetLibMap().SetMode(nameID, vID)); }

void ShaderInstanceBase::SetMode(string_view name) { SetVariantID(pLib->GetLibMap().SetMode(name, vID)); }

void ShaderInstanceBase::ResetMode() { SetVariantID(pLib->GetLibMap().ResetMode(vID)); }

void ShaderInstanceBase::ResetDefines() { SetVariantID(pLib->GetLibMap().ResetVariant(vID)); }

void ShaderInstanceBase::GetDefines(Vector<uint>& nameIDs) const { pLib->GetLibMap().GetDefines(vID, nameIDs); }
