#pragma once
#include <string_view>
#include "GfxException.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <wrl.h>
#include "UniqueObj.hpp"
#include "DynamicCollections.hpp"

namespace Replica::D3D11
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

	fquat QuatFromAxis(vec3 axis, float rad);
	size_t GetAlignedByteSize(size_t size, size_t alignment);
}