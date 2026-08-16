cmake_minimum_required(VERSION 3.28)
project(Stardust LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CLion / Ninja 构建目录默认在 cmake-build-*，与源码树分离
set(STARDUST_ROOT ${CMAKE_SOURCE_DIR})

add_subdirectory(ThirdParty)
add_subdirectory(Source)
