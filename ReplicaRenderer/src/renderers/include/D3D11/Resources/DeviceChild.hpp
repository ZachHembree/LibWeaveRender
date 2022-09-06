#pragma once
#include "D3DUtils.hpp"
#include <d3d11.h>

namespace Replica::D3D11
{
	class Device;
	class Context;

	/// <summary>
	/// Base class for types representing interfaces to device resources
	/// </summary>
	class DeviceChild : public MoveOnlyObjBase
	{
	public:
		/// <summary>
		/// Returns the device associated with the child object
		/// </summary>
		Device* GetDevice() const { return pDev; }

	protected:
		Device* pDev;

		DeviceChild() : pDev(nullptr) { }

		DeviceChild(Device& dev) : pDev(&dev) { }
	};
}