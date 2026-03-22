# NOTE: Use "Visual Studio 17 2022" generator for VS 2022
# Example: cmake -B build -G "Visual Studio 17 2022" -A x64
cmake_minimum_required(VERSION 3.14)

project(clear_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# VGLITE_USE_GL_BACKEND option (can be set from parent CMakeLists.txt or command line)
# Default to ON if not specified
if(NOT DEFINED VGLITE_USE_GL_BACKEND OR VGLITE_USE_GL_BACKEND STREQUAL "")
    set(VGLITE_USE_GL_BACKEND ON)
endif()

# Ensure it's a boolean
if(VGLITE_USE_GL_BACKEND)
    set(VGLITE_USE_GL_BACKEND ON CACHE BOOL "Use GL backend for rendering" FORCE)
else()
    set(VGLITE_USE_GL_BACKEND OFF CACHE BOOL "Use GL backend for rendering" FORCE)
endif()

message(STATUS "VGLITE_USE_GL_BACKEND: ${VGLITE_USE_GL_BACKEND}")

# Get absolute paths
get_filename_component(VGLITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
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
# Pass VGLITE_USE_GL_BACKEND to subdirectories
set(VGLITE_USE_GL_BACKEND ${VGLITE_USE_GL_BACKEND} PARENT_SCOPE)

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

# Link libraries (GLFW always needs OpenGL for window display)
target_link_libraries(clear_demo PRIVATE
    thorvg
    vg_lite_util
    png_static
    zlib_static
    glfw
    opengl32    # Always needed for GLFW window display
)

# Set VGLITE_USE_GL_BACKEND compile definition when enabled
if(VGLITE_USE_GL_BACKEND)
    target_compile_definitions(clear_demo PRIVATE VGLITE_USE_GL_BACKEND)
    message(STATUS "OpenGL backend enabled - ThorVG renders directly to GL FBO")
else()
    message(STATUS "OpenGL backend disabled - software rendering with texture upload")
endif()
