#pragma once
#include "RepWinException.hpp"

#define GFX_THROW_FAILED(hr) if (FAILED(hr)) { throw RepWinException(__LINE__, __FILE__, hr); }
