#pragma once
// spanstream is only available in C++23 and later
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
#include <strstream>

#include <unordered_map>
#include <list>
#include <stack>
#include <memory>
#include <optional>
#include <algorithm>
#include <format>
#include <concepts>
#include <type_traits>

#include "WeaveUtils/GlobalUtils.hpp"
#include "WeaveUtils/Math.hpp"
#include "WeaveEffects/ShaderLibBuilder/WaveConfig.hpp"
#include "WeaveEffects/EffectParseException.hpp"
#include "WeaveUtils/Logger.hpp"