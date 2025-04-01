#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ReplicaUtils/Int.hpp"

namespace Replica
{
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

	constexpr double g_TauD = 2.0 * glm::pi<double>();
	constexpr double g_PiD = glm::pi<double>();
	constexpr float g_Tau = 2.0f * glm::pi<float>();
	constexpr float g_Pi = glm::pi<float>();
	
	fquat QuatFromAxis(vec3 axis, float rad);
	size_t GetAlignedByteSize(size_t size, size_t alignment);
}