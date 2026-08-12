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

# 统一导出给引擎目标
add_library(StardustThirdParty INTERFACE)
target_link_libraries(StardustThirdParty INTERFACE
    spdlog::spdlog
    glm::glm
    mimalloc-static
)
