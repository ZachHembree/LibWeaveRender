#include "pch.hpp"
#include "D3D11/InternalD3D11.hpp"
#include "D3D11/Resources/DeviceChild.hpp"
#include "D3D11/Device.hpp"

using namespace Weave::D3D11;

/// <summary>
/// Returns the device associated with the child object
/// </summary>
Device& DeviceChild::GetDevice() const { D3D_ASSERT_MSG(pDev != nullptr, "Cannot get null reference."); return *pDev; }

/// <summary>
/// Returns reference to renderer using this object
/// </summary>
Renderer& DeviceChild::GetRenderer() const { return GetDevice().GetRenderer(); }

/// <summary>
/// Returns true if the object is valid and has been initialized
/// </summary>
bool DeviceChild::GetIsValid() const { return pDev != nullptr; }

DeviceChild::DeviceChild() : pDev(nullptr) { }

DeviceChild::DeviceChild(Device& dev) : pDev(&dev) { }
