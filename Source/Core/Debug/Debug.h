#pragma once

#include <cstdlib>
#include <source_location>

#include "Core/Logging/Logger.h"

inline void DebugBreakpoint()
{
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    __asm__ volatile("int $3");
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_trap();
#else
    std::abort();
#endif
}

inline void DebugAssert(const bool Condition, const std::source_location Location = std::source_location::current())
{
#if defined(NDEBUG)
    (void)Condition;
    (void)Location;
#else
    if (!Condition) [[unlikely]]
    {
        Logger::Log(LogLevel::Assert, Logcat::Assert, Location, "[Assert] DebugAssert failed");
        DebugBreakpoint();
    }
#endif
}

inline void Assert(const bool Condition, const std::source_location Location = std::source_location::current())
{
    if (!Condition) [[unlikely]]
    {
        Logger::Log(LogLevel::Assert, Logcat::Assert, Location, "[Assert] Assertion failed");
        DebugBreakpoint();
        std::abort();
    }
}
