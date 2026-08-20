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

# ---------------------------------------------------------------------------
# SDL3（静态链入，关测试/示例/安装）
# ---------------------------------------------------------------------------
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# slang；禁用会触发网络下载的选项）
# ---------------------------------------------------------------------------
set(SLANG_ENABLE_CUDA OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_OPTIX OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_NVAPI OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_AFTERMATH OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_DX_ON_VK OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_GFX OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANGD OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANGC OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SLANGI OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_REPLAYER OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_DXIL OFF CACHE BOOL "" FORCE)
set(SLANG_EXCLUDE_DAWN ON CACHE BOOL "" FORCE)
set(SLANG_EXCLUDE_TINT ON CACHE BOOL "" FORCE)
set(SLANG_ENABLE_MIMALLOC OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SPIRV_TOOLS_MIMALLOC OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_SPLIT_DEBUG_INFO OFF CACHE BOOL "" FORCE)
set(SLANG_SLANG_LLVM_FLAVOR DISABLE CACHE STRING "" FORCE)
set(SLANG_LIB_TYPE STATIC CACHE STRING "" FORCE)
add_subdirectory(slang EXCLUDE_FROM_ALL)

# slang 在 _WIN32 下入口是 wmain；MinGW 默认找 main，需 -municode
if (MINGW)
    foreach (SlangExe IN ITEMS slang-bootstrap slangc)
        if (TARGET ${SlangExe})
            target_link_options(${SlangExe} PRIVATE -municode)
        endif ()
    endforeach ()
endif ()

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
    SDL3::SDL3
    slang
)
target_compile_definitions(StardustThirdParty INTERFACE
    VK_NO_PROTOTYPES
    VMA_STATIC_VULKAN_FUNCTIONS=0
    VMA_DYNAMIC_VULKAN_FUNCTIONS=1
)
