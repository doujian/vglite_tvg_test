# VGLite Examples Top-Level Configuration
# Supports multi-demo builds with auto-discovery

cmake_minimum_required(VERSION 3.14)
project(vglite_examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Enable dual ThorVG build mode
# ============================================================================
set(VGLITE_BUILD_DUAL_THORVG ON CACHE BOOL "Build both GL and SW ThorVG" FORCE)
message(STATUS "VGLITE_BUILD_DUAL_THORVG: ${VGLITE_BUILD_DUAL_THORVG}")

# Get absolute path to VGLite root
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
# Build ThorVG (dual version) and utility libraries
# ============================================================================
add_subdirectory(${VGLITE_ROOT}/tests/thorvg thorvg)
add_subdirectory(${VGLITE_ROOT}/tests/utils utils)

# ============================================================================
# Load demo template
# ============================================================================
include(cmake/DemoTemplate.cmake)

# ============================================================================
# Auto-discover and include all demos
# ============================================================================
file(GLOB DEMO_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/demos/*")
set(DEMO_COUNT 0)

foreach(DEMO_DIR ${DEMO_DIRS})
    if(IS_DIRECTORY ${DEMO_DIR})
        get_filename_component(DEMO_NAME ${DEMO_DIR} NAME)
        message(STATUS "Discovering demo: ${DEMO_NAME}")
        add_subdirectory(${DEMO_DIR})
        math(EXPR DEMO_COUNT "${DEMO_COUNT} + 1")
    endif()
endforeach()

message(STATUS "")
message(STATUS "Examples configuration complete!")
message(STATUS "  - Discovered demos: ${DEMO_COUNT}")
message(STATUS "  - Each demo has _gl and _sw variants")
message(STATUS "")
