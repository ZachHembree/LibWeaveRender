#pragma once
#include "WeaveUtils/Int.hpp"
#include <type_traits>

// Default constructor and copy/move operator utils
// Delete copy/move
#define MAKE_NO_COPY(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;  
// Deletes move operations
#define MAKE_NO_MOVE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;
// Default copy/move
#define MAKE_DEF_COPY(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;  
// Defaults move operations
#define MAKE_DEF_MOVE(Type) \
    Type(Type&&) noexcept = default; \
    Type& operator=(Type&&) noexcept = default;
// Defaults move and copy operators
#define MAKE_DEF_MOVE_COPY(Type) \
    MAKE_DEF_COPY(Type) \
    MAKE_DEF_MOVE(Type)
// Default move or copy only
#define MAKE_MOVE_ONLY(Type) \
    MAKE_NO_COPY(Type) \
    MAKE_DEF_MOVE(Type)
// Defaults copy operations and deletes move operations
#define MAKE_COPY_ONLY(Type) \
    MAKE_DEF_COPY(Type)  \
    MAKE_NO_MOVE(Type)
// Deletes copy and move operations
#define MAKE_IMMOVABLE(Type) \
    MAKE_NO_COPY(Type) \
    MAKE_NO_MOVE(Type)
// Default compare
#define USE_DEFAULT_CMP(Type) bool operator==(const Type& rhs) const = default; \
    bool operator!=(const Type& rhs) const = default;
// Default copy, move and compare
#define MAKE_DEF_ALL(Type) \
    MAKE_DEF_MOVE_COPY(Type) \
    USE_DEFAULT_CMP(Type) 

/*
    Allocates temporary stack allocated array with alloca and assigns it to the given pointer.

    Alloca arrays are allocated on the stack frame of the calling function. When that function
    returns, the array is deallocated.
*/
#define ALLOCA_ARR_SET(ARR_PTR, COUNT, TYPE)\
    do { \
    ARR_PTR = (TYPE*)alloca(COUNT * sizeof(TYPE)); \
    } while(0)

/*
    Allocates temporary stack allocated array with alloca, fills is with nulls, and assigns 
    it to the given pointer

    Alloca arrays are allocated on the stack frame of the calling function. When that function
    returns, the array is deallocated.
*/
#define ALLOCA_ARR_SET_NULL(ARR_PTR, COUNT, TYPE) \
    do { \
    TYPE* ARR_PTR##_INT = (TYPE*)alloca(COUNT * sizeof(TYPE)); \
    memset( ARR_PTR##_INT, 0u, (COUNT * sizeof(TYPE)) ); \
    ARR_PTR = ARR_PTR##_INT; \
    } while(0)

// Concept to constrain templates to enum types
template <typename T>
concept EnumType = std::is_enum_v<T>;

// Bitwise AND
template <EnumType T>
inline constexpr T operator&(const T& a, const T& b) {
    using T_INT = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<T_INT>(a) & static_cast<T_INT>(b));
}

template <EnumType T>
inline constexpr T& operator&=(T& a, const T& b) {
    using T_INT = std::underlying_type_t<T>;
    a = static_cast<T>(static_cast<T_INT>(a) & static_cast<T_INT>(b));
    return a;
}

// Bitwise OR
template <EnumType T>
inline constexpr T operator|(const T& a, const T& b) {
    using T_INT = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<T_INT>(a) | static_cast<T_INT>(b));
}

template <EnumType T>
inline constexpr T& operator|=(T& a, const T& b) {
    using T_INT = std::underlying_type_t<T>;
    a = static_cast<T>(static_cast<T_INT>(a) | static_cast<T_INT>(b));
    return a;
}

// Bitwise Negation
template <EnumType T>
inline constexpr T operator~(const T& a) {
    using T_INT = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<T_INT>(a));
}

// Left Shift
template <EnumType T, std::integral U>
inline constexpr T operator<<(const T& a, U b) {
    using T_INT = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<T_INT>(a) << b);
}

template <EnumType T, std::integral U>
inline constexpr T& operator<<=(T& a, U b) {
    using T_INT = std::underlying_type_t<T>;
    a = static_cast<T>(static_cast<T_INT>(a) << b);
    return a;
}

// Right Shift
template <EnumType T, std::integral U>
inline constexpr T operator>>(const T& a, U b) {
    using T_INT = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<T_INT>(a) >> b);
}

template <EnumType T, std::integral U>
inline constexpr T& operator>>=(T& a, U b) {
    using T_INT = std::underlying_type_t<T>;
    a = static_cast<T>(static_cast<T_INT>(a) >> b);
    return a;
}
