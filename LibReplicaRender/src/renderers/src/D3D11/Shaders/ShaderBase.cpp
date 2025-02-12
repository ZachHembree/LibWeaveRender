#include "pch.hpp"
#include "ShaderLibGen/ShaderData.hpp"
#include "ReplicaInternalD3D11.hpp"

using namespace Replica::Effects;
using namespace Replica::D3D11;
using namespace Replica;

ShaderBase::ShaderBase() : 
	DeviceChild(),
	isBound(false),
	pCtx(nullptr)
{ }

ShaderBase::ShaderBase(Device& dev) :
	DeviceChild(dev),
	pCtx(&dev.GetContext()),
	isBound(false)
{ }

ShaderBase::ShaderBase(Device& dev, const ShaderDef& def) :
	DeviceChild(dev),
	pCtx(&dev.GetContext()),
	isBound(false),
	samplers(def.res, ShaderTypes::Sampler),
	textures(def.res, ShaderTypes::Texture, ShaderTypes::RandomWrite),
	cBuffers(def.constBufs.GetLength())
{
	for (int i = 0; i < def.constBufs.GetLength(); i++)
	{
		const ConstBufLayout& layout = def.constBufs[i];
		constants.emplace(string_view(layout.name), ConstantMap(layout));
		cBuffers[i] = ConstantBuffer(dev, layout.size);
	}
}

ShaderBase::ShaderBase(ShaderBase&& other) noexcept :
	DeviceChild(std::move(other)),
	constants(std::move(other.constants)),
	samplers(std::move(other.samplers)),
	textures(std::move(other.textures)),
	cBuffers(std::move(other.cBuffers)),
	pCtx(other.pCtx),
	isBound(other.isBound)
{
	other.pCtx = nullptr;
}

ShaderBase& ShaderBase::operator=(ShaderBase&& other) noexcept
{
	DeviceChild::operator=(std::move(other));
	this->constants = std::move(other.constants);
	this->samplers = std::move(other.samplers);
	this->textures = std::move(other.textures);
	this->cBuffers = std::move(other.cBuffers);
	this->pCtx = other.pCtx;
	this->isBound = other.isBound;
	other.pCtx = nullptr;

	return *this;
}

void ShaderBase::Bind() { Bind(pDev->GetContext()); }

/// <summary>
/// Returns true if the shader is bound
/// </summary>
bool ShaderBase::GetIsBound() { return isBound; }

void ShaderBase::SetBuffer(string_view name, const byte* pSrc, const size_t size)
{
	const auto it = constants.find(name);

	if (it != constants.end())
	{
		it->second.SetData(pSrc, size);
	}
}

void ShaderBase::SetSampler(string_view name, Sampler& samp)
{
	samplers.SetResource(name, samp.Get());
}

void ShaderBase::SetTexture(string_view name, ITexture2D& tex)
{
	textures.SetResource(name, tex.GetSRV());
}
