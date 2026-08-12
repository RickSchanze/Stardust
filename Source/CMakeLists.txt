set(STARDUST_SOURCES
    Main.cpp
)

add_executable(Stardust ${STARDUST_SOURCES})

target_link_libraries(Stardust PRIVATE StardustThirdParty)

target_compile_features(Stardust PRIVATE cxx_std_26)

# GCC 16+: C++26 静态反射随 -std=c++26 启用
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Stardust PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
