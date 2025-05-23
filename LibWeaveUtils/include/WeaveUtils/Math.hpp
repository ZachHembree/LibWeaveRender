#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE 
#define GLM_FORCE_LEFT_HANDED

#include "GlobalUtils.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Weave
{
	template<typename T, glm::length_t N>
	using tvec = glm::vec<N, T, glm::qualifier::defaultp>;

	template<typename T>
	using tvec2 = glm::tvec2<T>;

	template<typename T>
	using tvec3 = glm::tvec3<T>;

	template<typename T>
	using tvec4 = glm::tvec4<T>;

	using uivec2 = glm::tvec2<uint>;
	using uivec3 = glm::tvec3<uint>;
	using uivec4 = glm::tvec4<uint>;

	using bvec2 = glm::bvec2;
	using bvec3 = glm::bvec3;
	using bvec4 = glm::bvec4;
	
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

	/// <summary>
	/// Returns unsigned difference (a - b) between two unsigned integers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(size_t a, size_t b);

	/// <summary>
	/// Returns unsigned difference (a - b) between two pointers, clamped to zero.
	/// </summary>
	size_t UnsignedDelta(const void* a, const void* b);

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(size_t a, size_t b);

	/// <summary>
	/// Returns signed difference (a - b) between two unsigned integers.
	/// </summary>
	long Delta(const void* a, const void* b);

	template <typename VecT>
	concept IsFixedLengthVec = requires
	{
		{ VecT::length() } -> std::convertible_to<std::size_t>;
	};

	/// <summary>
	/// Concept that requires two vector types to be equal length and comprised of members with 
	/// defined inequality operators.
	/// </summary>
	template<typename LVecT, typename RVecT>
	concept IsComparableVector = requires(const LVecT& lhs, const RVecT& rhs, std::size_t i)
	{
		{ lhs[i] <=> rhs[i] } -> std::convertible_to<std::partial_ordering>;

		requires IsFixedLengthVec<LVecT>;
		requires IsFixedLengthVec<RVecT>;
		requires LVecT::length() == RVecT::length();
	};

    /// <summary>
    /// Member-wise less than operator for equal-length vectors. Returns a vector of booleans of equal
	/// length for each corresponding pair of values compared.
    /// </summary>
    template<typename LVecT, typename RVecT, typename bvecT = tvec<bool, LVecT::length()>>
	requires IsComparableVector<LVecT, RVecT>
    inline constexpr bvecT operator<(const LVecT& lhs, const RVecT& rhs)
    {
		constexpr std::size_t N = LVecT::length();
		const auto GetBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bvecT
		{
			return bvecT{ (lhs[Is] < rhs[Is])... };
		};

		return GetBools(std::make_index_sequence<N>{});
    }

	/// <summary>
	/// Member-wise greater than operator for equal-length vectors. Returns a vector of booleans of equal
	/// length for each corresponding pair of values compared.
	/// </summary>
	template<typename LVecT, typename RVecT, typename bvecT = tvec<bool, LVecT::length()>>
	requires IsComparableVector<LVecT, RVecT>
	inline constexpr bvecT operator>(const LVecT& lhs, const RVecT& rhs)
	{
		constexpr std::size_t N = LVecT::length();
		const auto GetBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bvecT
		{
			return bvecT{ (lhs[Is] > rhs[Is])... };
		};

		return GetBools(std::make_index_sequence<N>{});
	}

	/// <summary>
	/// Member-wise less than or equal operator for equal-length vectors. Returns a vector of booleans of equal
	/// length for each corresponding pair of values compared.
	/// </summary>
	template<typename LVecT, typename RVecT, typename bvecT = tvec<bool, LVecT::length()>>
	requires IsComparableVector<LVecT, RVecT>
	inline constexpr bvecT operator<=(const LVecT& lhs, const RVecT& rhs)
	{
		constexpr std::size_t N = LVecT::length();
		const auto GetBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bvecT
		{
			return bvecT{ (lhs[Is] <= rhs[Is])... };
		};

		return GetBools(std::make_index_sequence<N>{});
	}

	/// <summary>
	/// Member-wise greater than or equal operator for equal-length vectors. Returns a vector of booleans of equal
	/// length for each corresponding pair of values compared.
	/// </summary>
	template<typename LVecT, typename RVecT, typename bvecT = tvec<bool, LVecT::length()>>
	requires IsComparableVector<LVecT, RVecT>
	inline constexpr bvecT operator>=(const LVecT& lhs, const RVecT& rhs)
	{
		constexpr std::size_t N = LVecT::length();
		const auto GetBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bvecT
		{
			return bvecT{ (lhs[Is] >= rhs[Is])... };
		};

		return GetBools(std::make_index_sequence<N>{});
	}

	/// <summary>
	/// Returns true if ALL booleans in the vector are true
	/// </summary>
	template<typename bvecT>
	requires IsFixedLengthVec<bvecT>
	inline constexpr bool AllTrue(const bvecT& bvec)
	{
		constexpr std::size_t N = bvecT::length();
		const auto CollapseBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bool
		{
			return (bvec[Is] && ... );
		};

		return CollapseBools(std::make_index_sequence<N>{});
	}

	/// <summary>
	/// Returns true if any ONE boolean in the vector is true
	/// </summary>
	template<typename bvecT>
	requires IsFixedLengthVec<bvecT>
	inline constexpr bool AnyTrue(const bvecT& bvec)
	{
		constexpr std::size_t N = bvecT::length();
		const auto CollapseBools = [&]<std::size_t... Is>(std::index_sequence<Is...>) constexpr -> bool
		{
			return (bvec[Is] || ...);
		};

		return CollapseBools(std::make_index_sequence<N>{});
	}
}