set(STARDUST_SOURCES
    Main.cpp
    Core/Logging/Logger.cpp
)

add_executable(Stardust ${STARDUST_SOURCES})

# Source/ 为 include 根
target_include_directories(Stardust PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Stardust PRIVATE StardustThirdParty)

target_compile_features(Stardust PRIVATE cxx_std_26)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Stardust PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -freflection
    )
endif ()
