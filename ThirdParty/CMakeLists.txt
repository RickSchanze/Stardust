# 第三方均为 git submodule，禁止 FetchContent / 下载式引入

# ---------------------------------------------------------------------------
# spdlog
# ---------------------------------------------------------------------------
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# glm
# ---------------------------------------------------------------------------
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glm EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# mimalloc
# ---------------------------------------------------------------------------
set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(MI_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MI_INSTALL_TOPLEVEL OFF CACHE BOOL "" FORCE)
add_subdirectory(mimalloc EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# tracy（与 CoreConfig.h 的 STARDUST_ENABLE_PROFILING 对齐）
# ---------------------------------------------------------------------------
set(TRACY_ENABLE ON CACHE BOOL "" FORCE)
set(TRACY_ON_DEMAND ON CACHE BOOL "" FORCE)
set(TRACY_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(tracy EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# Vulkan-Headers（编译期头文件，不依赖本机 SDK 路径）
# ---------------------------------------------------------------------------
set(VULKAN_HEADERS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(VULKAN_HEADERS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(VULKAN_HEADERS_ENABLE_MODULE OFF CACHE BOOL "" FORCE)
add_subdirectory(Vulkan-Headers EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# volk：动态加载，禁止链接 vulkan-1
# ---------------------------------------------------------------------------
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR CACHE STRING "" FORCE)
endif ()
set(VOLK_INSTALL OFF CACHE BOOL "" FORCE)
set(VULKAN_HEADERS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers" CACHE PATH "" FORCE)
add_subdirectory(volk EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# VMA
# ---------------------------------------------------------------------------
set(VMA_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(VulkanMemoryAllocator EXCLUDE_FROM_ALL)

# 统一导出给引擎目标
add_library(StardustThirdParty INTERFACE)
target_link_libraries(StardustThirdParty INTERFACE
    spdlog::spdlog
    glm::glm
    mimalloc-static
    TracyClient
    Vulkan::Headers
    volk
    GPUOpen::VulkanMemoryAllocator
)
target_compile_definitions(StardustThirdParty INTERFACE
    VK_NO_PROTOTYPES
    VMA_STATIC_VULKAN_FUNCTIONS=0
    VMA_DYNAMIC_VULKAN_FUNCTIONS=1
)
