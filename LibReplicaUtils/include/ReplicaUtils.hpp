#pragma once
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

#define MAKE_NO_COPY(Type)            \
    Type(const Type&) = delete;         \
    Type& operator=(const Type&) = delete;  

#define MAKE_NO_MOVE(Type)            \
    Type(Type&&) = delete;             \
    Type& operator=(Type&&) = delete;

#define MAKE_MOVE_ONLY(Type)            \
    Type(const Type&) = delete;         \
    Type& operator=(const Type&) = delete;  \
    Type(Type&&) = default;             \
    Type& operator=(Type&&) = default;

#define MAKE_COPY_ONLY(Type)            \
    Type(const Type&) = default;         \
    Type& operator=(const Type&) = default;  \
    Type(Type&&) = delete;             \
    Type& operator=(Type&&) = delete;

#define MAKE_IMMOVABLE(Type)            \
    Type(const Type&) = delete;         \
    Type& operator=(const Type&) = delete;  \
    Type(Type&&) = delete;             \
    Type& operator=(Type&&) = delete;

#include <string>
#include <string_view>
#include "internal/Utils.hpp"
#include "DynamicCollections.hpp"
#include "Stopwatch.hpp"
#include "Span.hpp"
#include "TextBlock.hpp"
#include "DataBufferHandle.hpp"
#include "TextUtils.hpp"
#include "RepException.hpp"