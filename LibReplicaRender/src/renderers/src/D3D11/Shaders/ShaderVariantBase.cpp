#include "pch.hpp"
#include "ReplicaInternalD3D11.hpp"
#include "D3D11/Shaders/ShaderVariantBase.hpp"
#include "D3D11/ShaderLibrary.hpp"

using namespace Replica;
using namespace Replica::D3D11;

string_view ShaderVariantBase::GetName() const { return GetDefinition().name; }

const ShaderLibrary& ShaderVariantBase::GetLibrary() const { GFX_ASSERT(pLib != nullptr, "Cannot access null shader library"); return *pLib; }

ShaderLibrary& ShaderVariantBase::GetLibrary() { GFX_ASSERT(pLib != nullptr, "Cannot access null shader library"); return *pLib; }

const ShaderDef& ShaderVariantBase::GetDefinition() const { GFX_ASSERT(pDef != nullptr, "Cannot access null shader definition"); return *pDef; }

bool ShaderVariantBase::GetIsDefined(string_view define) const { return GetLibrary().GetLibMap().GetIsDefined(define, pDef->variantID); }

ShaderVariantBase::ShaderVariantBase() :
	DeviceChild(),
	pLib(nullptr),
	pDef(nullptr)
{ }

ShaderVariantBase::ShaderVariantBase(Device& dev, ShaderLibrary& lib, const ShaderDef& def) :
	DeviceChild(dev),
	pLib(&lib),
	pDef(&def)
{ }

ShaderVariantBase::ShaderVariantBase(ShaderVariantBase&& other) noexcept = default;

ShaderVariantBase& ShaderVariantBase::operator=(ShaderVariantBase&& other) noexcept = default;