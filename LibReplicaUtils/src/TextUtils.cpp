#include "pch.hpp"
#include "../include/TextUtils.hpp"
#include <stdarg.h>
#include <stdio.h>

namespace Replica
{
    void GetFmtString(string& text, const char* fmt, ...)
    {
        char buf[512];

        va_list pArgs;
        va_start(pArgs, fmt);
        vsnprintf(buf, 512, fmt, pArgs);
        va_end(pArgs);

        text.append(buf);
        text.append("\0");
    }

    string GetFmtString(const char* fmt, ...)
    {
        char buf[512];

        va_list pArgs;
        va_start(pArgs, fmt);
        vsnprintf(buf, 512, fmt, pArgs);
        va_end(pArgs);

        return string(buf);
    }
}