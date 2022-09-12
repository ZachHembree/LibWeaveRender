#pragma once
#include "../src/utils/include/RepLeanWin.h"

#include <string_view>
#include <wrl.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../src/utils/include/MoveOnlyBase.hpp"
#include "../src/utils/include/DynamicCollections.hpp"
#include "../src/utils/include/RepWinException.hpp"
#include "../src/utils/include/GfxException.hpp"

#define BITWISE_AND(T, T_INT) \
        inline T operator& (const T& a, const T& b)\
        {\
            return (T)(static_cast<T_INT>(a) & static_cast<T_INT>(b));\
        }\
        inline T& operator&=(T& a, const T& b)\
        {\
            a = (T)(static_cast<T_INT>(a) & static_cast<T_INT>(b));\
            return a;\
        }

#define BITWISE_OR(T, T_INT) \
        inline T operator| (const T& a, const T& b)\
        {\
            return (T)(static_cast<T_INT>(a) | static_cast<T_INT>(b));\
        }\
        inline T& operator|=(T& a, const T& b)\
        {\
            a = (T)(static_cast<T_INT>(a) | static_cast<T_INT>(b));\
            return a;\
        }

#define BITWISE_NEGATE(T, T_INT) \
        inline T operator~ (const T& a)\
        {\
            return (T)(~static_cast<T_INT>(a));\
        }

#define BITWISE_ALL(T, T_INT) BITWISE_AND(T, T_INT) BITWISE_OR(T, T_INT) BITWISE_NEGATE(T, T_INT) 

namespace Replica
{
	using std::string_view;
	using std::wstring_view;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	template<typename T>
	using tvec2 = glm::tvec1<T>;

	template<typename T>
	using tvec3 = glm::tvec3<T>;

	template<typename T>
	using tvec4 = glm::tvec4<T>;

	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	using vec4 = glm::vec4;
	using ivec2 = glm::ivec2;
	using ivec3 = glm::ivec3;
	using ivec4 = glm::ivec4;
	using fquat = glm::fquat;
	using color = glm::tvec4<byte>;

	using mat3 = glm::mat3;
	using mat3x3 = glm::mat3x3;
	using mat4 = glm::mat4;
	using mat4x4 = glm::mat4x4;

	typedef const wchar_t* WSTR;
	typedef const char* STR;
	typedef unsigned long ulong;
	typedef unsigned int uint;

	constexpr double g_TauD = 2.0 * glm::pi<double>();
	constexpr double g_PiD = glm::pi<double>();
	constexpr float g_Tau = 2.0f * glm::pi<float>();
	constexpr float g_Pi = glm::pi<float>();

	fquat QuatFromAxis(vec3 axis, float rad);
	size_t GetAlignedByteSize(size_t size, size_t alignment);
}