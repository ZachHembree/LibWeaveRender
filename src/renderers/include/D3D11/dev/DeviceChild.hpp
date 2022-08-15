#pragma once
#include "D3D11/D3DUtils.hpp"
#include <d3d11.h>

namespace Replica::D3D11
{
	class Device;

	class DeviceChild : protected UniqueObjBase
	{
	public:
		/// <summary>
		/// Returns the device associated with the child object
		/// </summary>
		const Device* GetDevice() { return pDev; }

	protected:
		Device* pDev;

		DeviceChild(Device* pDev) : pDev(pDev) { }

	};
}