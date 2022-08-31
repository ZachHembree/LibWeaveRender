#include "D3D11/dev/ShaderBase.hpp"
#include "D3D11/dev/Device.hpp"
#include "D3D11/dev/ConstantBuffer.hpp"
#include "D3D11/dev/ConstantMap.hpp"

using namespace Replica::D3D11;

ShaderBase::ShaderBase(Device& dev) :
	DeviceChild(&dev),
	pCtx(&dev.GetContext()),
	isBound(false)
{ }

ShaderBase::ShaderBase(Device& dev, const ConstantMapDef& cDef) :
	ShaderBase(dev)
{
	if (cDef.GetStride() > 0)
	{ 
		constants = ConstantMap(cDef);
		cBuf = ConstantBuffer(dev, constants.GetStride());
	}
}

ShaderBase::ShaderBase(ShaderBase&& other) noexcept :
	DeviceChild(std::move(other)),
	constants(std::move(other.constants)),
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
	this->cBuf = std::move(other.cBuf);
	this->pCtx = other.pCtx;
	this->isBound = other.isBound;
	other.pCtx = nullptr;

	return *this;
}

void ShaderBase::Bind() { Bind(pDev->GetContext()); }