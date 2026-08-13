#pragma once

#define STARDUST_LOG_WITH_FILE_INFO 1
#define STARDUST_ENABLE_CONTAINER_CHECK 1

inline constexpr int gMinimumLogLevel = 0;
inline constexpr bool gEnableContainerCheck = STARDUST_ENABLE_CONTAINER_CHECK != 0;
