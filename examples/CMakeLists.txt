# NOTE: Use "Visual Studio 17 2022" generator for VS 2022
# Example: cmake -B build -G "Visual Studio 17 2022" -A x64
cmake_minimum_required(VERSION 3.14)

project(clear_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get absolute paths
get_filename_component(VGLITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

message(STATUS "VGLITE_ROOT: ${VGLITE_ROOT}")

# ============================================================================
# FetchContent for GLFW
# ============================================================================
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# ============================================================================
# Build ThorVG and utility libraries
# ============================================================================
add_subdirectory(${VGLITE_ROOT}/tests/thorvg thorvg)
add_subdirectory(${VGLITE_ROOT}/tests/utils utils)

# ============================================================================
# Demo executable
# ============================================================================
add_executable(clear_demo
    clear_demo/clear_demo.cpp
)

# Include directories
target_include_directories(clear_demo PRIVATE
    ${VGLITE_ROOT}                            # Parent vglite_thorvg directory (for vg_lite.h, vglite_config.h)
    ${VGLITE_ROOT}/libs/thorvg-1.0.2/inc      # ThorVG 1.0.2 library headers
    ${VGLITE_ROOT}/tests/utils/               # vg_lite_util header
)

# Link against parent vglite_thorvg sources
target_sources(clear_demo PRIVATE
    ${VGLITE_ROOT}/vg_lite_tvg.cpp
    ${VGLITE_ROOT}/vg_lite_matrix.c
)

# Compile definitions
target_compile_definitions(clear_demo PRIVATE
    TVG_STATIC
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    NOMINMAX
)

# Link libraries
target_link_libraries(clear_demo PRIVATE
    thorvg
    vg_lite_util
    png_static
    zlib_static
    glfw
    opengl32
)
