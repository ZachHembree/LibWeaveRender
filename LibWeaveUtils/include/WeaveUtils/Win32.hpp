#pragma once
#include "WeaveUtils/WinUtils.hpp"
#include "MinWindow.hpp"
#include "WeaveUtils/WindowComponentBase.hpp"
#include <wrl/client.h>

namespace Weave
{
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
}