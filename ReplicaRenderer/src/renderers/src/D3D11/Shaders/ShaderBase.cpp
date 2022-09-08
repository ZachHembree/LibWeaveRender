#include "D3D11/Shaders/ShaderBase.hpp"
#include "D3D11/Device.hpp"
#include "D3D11/Resources/ConstantBuffer.hpp"
#include "D3D11/Resources/ConstantMap.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Resources/Texture2D.hpp"

using namespace Replica::D3D11;

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

ShaderBase::ShaderBase(Device& dev, const ShaderDefBase& def) :
	DeviceChild(dev),
	pCtx(&dev.GetContext()),
	isBound(false),
	samplers(def.samplerMap),
	textures(def.textureMap)
{
	if (def.constMap.GetStride() > 0)
	{ 
		constants = ConstantMap(def.constMap);
		cBuf = ConstantBuffer(dev, constants.GetStride());
	}
}

ShaderBase::ShaderBase(ShaderBase&& other) noexcept :
	DeviceChild(std::move(other)),
	constants(std::move(other.constants)),
	samplers(std::move(other.samplers)),
	textures(std::move(other.textures)),
	cBuf(std::move(other.cBuf)),
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
	this->cBuf = std::move(other.cBuf);
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

void ShaderBase::SetSampler(wstring_view name, Sampler& samp)
{
	samplers.SetResource(name, samp.Get());
}

void ShaderBase::SetTexture(wstring_view name, Texture2D& tex)
{
	textures.SetResource(name, tex.GetSRV());
}
