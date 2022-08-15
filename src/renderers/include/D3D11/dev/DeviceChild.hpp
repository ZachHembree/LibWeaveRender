#pragma once
#include "D3D11/dev/Device.hpp"
#include "UniqueObj.hpp"

namespace Replica::D3D11
{
	class Device::Child : protected UniqueObjBase
	{ };
}