#pragma once

#define STARDUST_LOG_IMPL(LevelName, Category, Format, ...)                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if constexpr (std::to_underlying(LogLevel::LevelName) >= gMinimumLogLevel)                                     \
        {                                                                                                              \
            ::Logger::Log(LogLevel::LevelName,                                                                         \
                          ::Logcat::Category,                                                                          \
                          std::source_location::current(),                                                             \
                          "[" #Category "] " Format __VA_OPT__(, ) __VA_ARGS__);                                       \
        }                                                                                                              \
    } while (0)

#define LogDebug(Category, Format, ...) STARDUST_LOG_IMPL(Debug, Category, Format __VA_OPT__(, ) __VA_ARGS__)
#define LogInfo(Category, Format, ...) STARDUST_LOG_IMPL(Info, Category, Format __VA_OPT__(, ) __VA_ARGS__)
#define LogWarn(Category, Format, ...) STARDUST_LOG_IMPL(Warn, Category, Format __VA_OPT__(, ) __VA_ARGS__)
#define LogError(Category, Format, ...) STARDUST_LOG_IMPL(Error, Category, Format __VA_OPT__(, ) __VA_ARGS__)
#define LogCritical(Category, Format, ...) STARDUST_LOG_IMPL(Critical, Category, Format __VA_OPT__(, ) __VA_ARGS__)
#define LogAssert(Category, Format, ...) STARDUST_LOG_IMPL(Assert, Category, Format __VA_OPT__(, ) __VA_ARGS__)
