#pragma once

#include <memory>
#include <source_location>
#include <utility>

#include <spdlog/spdlog.h>

#include "Core/CoreConfig.h"
#include "Core/Logging/Logcat.h"

enum class LogLevel
{
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
    Critical = 4,
};

/**
 * 不使用 Singleton：Singleton 依赖 Logger，避免循环依赖。
 * 业务侧请用 LogDebug / LogInfo / … 宏（见 LogMacros.h）。
 */
class Logger
{
public:
    static void EnsureInitialized();

    [[nodiscard]] static spdlog::logger& Get();

    template <typename... Args>
    static void Log(
        LogLevel InLevel,
        Logcat InLogcat,
        const std::source_location& InLocation,
        spdlog::format_string_t<Args...> InFormat,
        Args&&... InArgs)
    {
        EnsureInitialized();
        (void)InLogcat; // 保留分类，供后续按 Logcat 过滤 / 分流
        mLogger->log(
            spdlog::source_loc{
                InLocation.file_name(),
                static_cast<int>(InLocation.line()),
                InLocation.function_name()},
            ToSpdlogLevel(InLevel),
            InFormat,
            std::forward<Args>(InArgs)...);
    }

private:
    [[nodiscard]] static spdlog::level::level_enum ToSpdlogLevel(LogLevel InLevel);

    static inline std::shared_ptr<spdlog::logger> mLogger;
};

#include "Core/Logging/LogMacros.h"
