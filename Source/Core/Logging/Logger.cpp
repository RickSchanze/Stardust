#include "Core/Logging/Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

void Logger::EnsureInitialized()
{
    if (mLogger != nullptr)
    {
        return;
    }

    mLogger = spdlog::stdout_color_mt("Stardust");
    mLogger->set_level(ToSpdlogLevel(static_cast<LogLevel>(gMinimumLogLevel)));

    // %^...%$ 整行着色；%s 短文件名；%# 行号
#if STARDUST_LOG_WITH_FILE_INFO
    mLogger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%L] [%s:%#] %v%$");
#else
    mLogger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%L] %v%$");
#endif
}

spdlog::logger& Logger::Get()
{
    EnsureInitialized();
    return *mLogger;
}

spdlog::level::level_enum Logger::ToSpdlogLevel(LogLevel InLevel)
{
    switch (InLevel)
    {
    case LogLevel::Debug:
        return spdlog::level::debug;
    case LogLevel::Info:
        return spdlog::level::info;
    case LogLevel::Warn:
        return spdlog::level::warn;
    case LogLevel::Error:
        return spdlog::level::err;
    case LogLevel::Critical:
        return spdlog::level::critical;
    }
    return spdlog::level::info;
}
