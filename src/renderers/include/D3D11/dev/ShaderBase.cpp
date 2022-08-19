#include "D3D11/dev/ShaderBase.hpp"
#include "D3D11/dev/Device.hpp"

using namespace Replica::D3D11;

ShaderBase::ShaderBase(Device& dev) : 
	DeviceChild(&dev), 
	pCtx(&dev.GetContext()),
	isBound(false)
{ }

void ShaderBase::Bind() { Bind(pDev->GetContext()); }