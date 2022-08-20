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
	constants = ConstantMap(cDef);
	cBuf = ConstantBuffer(dev, constants.GetStride());
}

void ShaderBase::Bind() { Bind(pDev->GetContext()); }