#pragma once
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

#define BITSHIFT_LEFT(T, T_INT) \
        inline T operator<<(const T& a, T_INT b)\
        {\
            return (T)(static_cast<T_INT>(a) << b);\
        }\
        inline T& operator<<=(T& a, T_INT b)\
        {\
            a = (T)(static_cast<T_INT>(a) << b); \
            return a;\
        }

#define BITSHIFT_RIGHT(T, T_INT) \
        inline T operator>>(const T& a, T_INT b)\
        {\
            return (T)(static_cast<T_INT>(a) >> b);\
        }\
        inline T& operator>>=(T& a, T_INT b)\
        {\
            a = (T)(static_cast<T_INT>(a) >> b);\
            return a;\
        }

#define BITSHIFT_ALL(T, T_INT) \
    BITSHIFT_LEFT(T, T_INT) BITSHIFT_RIGHT(T, T_INT)

#define BITWISE_ALL(T, T_INT) BITWISE_AND(T, T_INT) BITWISE_OR(T, T_INT) BITWISE_NEGATE(T, T_INT) \
    BITSHIFT_ALL(T, T_INT)

#include <string>
#include <string_view>
#include "../src/utils/include/Utils.hpp"
#include "../src/utils/include/MoveOnlyBase.hpp"
#include "../src/utils/include/DynamicCollections.hpp"
#include "../src/utils/include/Stopwatch.hpp"
#include "../src/utils/include/Span.hpp"
#include "../src/utils/include/TextBlock.hpp"
#include "../src/utils/include/DataBufferHandle.hpp"

namespace Replica
{
	using std::string_view;
	using std::wstring_view;
    using std::string;
    using std::wstring;
}

#include "../src/utils/include/RepException.hpp"