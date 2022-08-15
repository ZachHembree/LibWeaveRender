#pragma once
#include "D3D11/dev/Device.hpp"
#include "UniqueObj.hpp"

namespace Replica::D3D11
{
	class Device::Child : protected UniqueObjBase
	{
	public:
		/// <summary>
		/// Returns the device associated with the child object
		/// </summary>
		const Device* GetDevice() { return pDev; }

	protected:
		const Device* pDev;

		Child(const Device* pDev) : pDev(pDev) { }

	};
}