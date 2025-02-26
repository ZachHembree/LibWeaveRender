#include "pch.hpp"
#include "D3D11/Resources/ResourceBase.hpp"
#include "D3D11/Resources/DeviceChild.hpp"
#include "D3D11/Device.hpp"

using namespace Replica;
using namespace Replica::D3D11;

ResourceBase::ResourceBase() : DeviceChild() {}

ResourceBase::ResourceBase(Device& dev) : DeviceChild(dev) {}

ResourceBase::ResourceBase(ResourceBase&& other) noexcept :
	DeviceChild(std::move(other))
{ }

ResourceBase& ResourceBase::operator=(ResourceBase&& other) noexcept
{
	DeviceChild::operator=(std::move(other));
	return *this;
}