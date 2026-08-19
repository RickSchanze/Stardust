#pragma once

#include <cstdint>
#include <cstring>
#include <source_location>

#include <tracy/TracyC.h>

#include "Core/CoreConfig.h"

// 用法PerformanceCounter _{};
class [[nodiscard]] PerformanceCounter
{
public:
    explicit PerformanceCounter(
        [[maybe_unused]] const char* Name = nullptr,
        [[maybe_unused]] const std::uint32_t Color = 0,
        [[maybe_unused]] const std::source_location Location = std::source_location::current()) noexcept
    {
#if STARDUST_ENABLE_PROFILING
        const char* File = Location.file_name();
        const char* Function = Location.function_name();
        const auto Line = Location.line();

        std::uint64_t Srcloc = 0;
        if (Name != nullptr && Name[0] != '\0')
        {
            Srcloc = ___tracy_alloc_srcloc_name(
                Line, File, std::strlen(File), Function, std::strlen(Function), Name, std::strlen(Name), Color);
        }
        else
        {
            Srcloc = ___tracy_alloc_srcloc(Line, File, std::strlen(File), Function, std::strlen(Function), Color);
        }

        mCtx = ___tracy_emit_zone_begin_alloc(Srcloc, 1);
#endif
    }

    ~PerformanceCounter()
    {
#if STARDUST_ENABLE_PROFILING
        ___tracy_emit_zone_end(mCtx);
#endif
    }

    PerformanceCounter(const PerformanceCounter&) = delete;
    PerformanceCounter(PerformanceCounter&&) = delete;
    PerformanceCounter& operator=(const PerformanceCounter&) = delete;
    PerformanceCounter& operator=(PerformanceCounter&&) = delete;

private:
#if STARDUST_ENABLE_PROFILING
    TracyCZoneCtx mCtx{};
#endif
};

inline void MarkFrame([[maybe_unused]] const char* Name = nullptr)
{
#if STARDUST_ENABLE_PROFILING
    ___tracy_emit_frame_mark(Name);
#endif
}
