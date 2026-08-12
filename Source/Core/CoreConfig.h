#pragma once

// 条件编译开关（宏仅允许用于 #if / #ifdef 等条件编译）
#define STARDUST_LOG_WITH_FILE_INFO 1

/**
 * 全局最低日志级别（与 LogLevel 底层值一致：Debug=0, Info=1, Warn=2, Error=3, Critical=4）。
 * - 日志宏里 if constexpr：低于此级别的调用在编译期剔除
 * - Logger 初始化时同步到 spdlog::set_level：运行时再挡一层
 */
inline constexpr int gMinimumLogLevel = 0;
