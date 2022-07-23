#pragma once
#include "RepWinException.hpp"

#define GFX_THROW_FAILED(x) { HRESULT hr = x; if (FAILED(hr)) { throw RepWinException(__LINE__, __FILE__, hr); } }
