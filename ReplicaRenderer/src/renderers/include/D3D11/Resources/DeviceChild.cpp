#include "ReplicaInternalD3D11.hpp"

using namespace Replica::D3D11;

/// <summary>
/// Returns the device associated with the child object
/// </summary>
Device& DeviceChild::GetDevice() const { GFX_ASSERT(pDev != nullptr, "Cannot get null reference."); return *pDev; }

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
