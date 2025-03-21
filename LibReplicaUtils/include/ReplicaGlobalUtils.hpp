#pragma once
#include "ReplicaInt.hpp"

// Enum bitwise utils
#define BITWISE_AND(T, T_INT) \
        inline constexpr T operator& (const T& a, const T& b)\
        {\
            return (T)(static_cast<T_INT>(a) & static_cast<T_INT>(b));\
        }\
        inline constexpr T& operator&=(T& a, const T& b)\
        {\
            a = (T)(static_cast<T_INT>(a) & static_cast<T_INT>(b));\
            return a;\
        }
#define BITWISE_OR(T, T_INT) \
        inline constexpr T operator| (const T& a, const T& b)\
        {\
            return (T)(static_cast<T_INT>(a) | static_cast<T_INT>(b));\
        }\
        inline constexpr T& operator|=(T& a, const T& b)\
        {\
            a = (T)(static_cast<T_INT>(a) | static_cast<T_INT>(b));\
            return a;\
        }
#define BITWISE_NEGATE(T, T_INT) \
        inline constexpr T operator~ (const T& a)\
        {\
            return (T)(~static_cast<T_INT>(a));\
        }
#define BITSHIFT_LEFT(T, T_INT) \
        inline constexpr T operator<<(const T& a, T_INT b)\
        {\
            return (T)(static_cast<T_INT>(a) << b);\
        }\
        inline constexpr T& operator<<=(T& a, T_INT b)\
        {\
            a = (T)(static_cast<T_INT>(a) << b); \
            return a;\
        }
#define BITSHIFT_RIGHT(T, T_INT) \
        inline constexpr T operator>>(const T& a, T_INT b)\
        {\
            return (T)(static_cast<T_INT>(a) >> b);\
        }\
        inline constexpr T& operator>>=(T& a, T_INT b)\
        {\
            a = (T)(static_cast<T_INT>(a) >> b);\
            return a;\
        }
#define BITSHIFT_ALL(T, T_INT) \
    BITSHIFT_LEFT(T, T_INT) BITSHIFT_RIGHT(T, T_INT)
#define BITWISE_ALL(T, T_INT) BITWISE_AND(T, T_INT) BITWISE_OR(T, T_INT) BITWISE_NEGATE(T, T_INT) \
    BITSHIFT_ALL(T, T_INT)
// Default constructor and copy/move operator utils
// Delete copy/move
#define MAKE_NO_COPY(Type) \
    Type(const Type&) = delete; \
    Type& operator=(const Type&) = delete;  
#define MAKE_NO_MOVE(Type) \
    Type(Type&&) = delete; \
    Type& operator=(Type&&) = delete;
// Default copy/move
#define MAKE_DEF_COPY(Type) \
    Type(const Type&) = default; \
    Type& operator=(const Type&) = default;  
#define MAKE_DEF_MOVE(Type) \
    Type(Type&&) = default; \
    Type& operator=(Type&&) = default;
#define MAKE_DEF_MOVE_COPY(Type) \
    MAKE_DEF_COPY(Type) \
    MAKE_DEF_MOVE(Type)
// Default move or copy only
#define MAKE_MOVE_ONLY(Type) \
    MAKE_NO_COPY(Type) \
    MAKE_DEF_MOVE(Type)
#define MAKE_COPY_ONLY(Type) \
    MAKE_DEF_COPY(Type)  \
    MAKE_NO_MOVE(Type)
// Delete copy and move
#define MAKE_IMMOVABLE(Type)            \
    Type(const Type&) = delete;         \
    Type& operator=(const Type&) = delete;  \
    Type(Type&&) = delete;             \
    Type& operator=(Type&&) = delete;
// Default compare
#define USE_DEFAULT_CMP(Type) bool operator==(const Type& rhs) const = default; \
    bool operator!=(const Type& rhs) const = default;
// Default copy, move and compare
#define MAKE_DEF_ALL(Type) \
    MAKE_DEF_MOVE_COPY(Type) \
    USE_DEFAULT_CMP(Type) 