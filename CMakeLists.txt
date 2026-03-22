# VGLite ThorVG Root CMakeLists.txt
# 
# Usage:
#   Build examples:  cmake -B build -G "Visual Studio 17 2022" -A x64 -DVGLITE_USE_GL_BACKEND=ON
#   Build tests:     cd tests && cmake -B build -G "Visual Studio 17 2022" -A x64
#                    (tests use root option if included)

cmake_minimum_required(VERSION 3.14)

project(vglite_thorvg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# VGLITE_USE_GL_BACKEND option
# ============================================================================
option(VGLITE_USE_GL_BACKEND "Use GL backend for rendering (enables OpenGL linking)" OFF)

message(STATUS "VGLITE_USE_GL_BACKEND: ${VGLITE_USE_GL_BACKEND}")

# ============================================================================
# Conditional OpenGL linking and compile definitions
# ============================================================================
if(VGLITE_USE_GL_BACKEND)
    message(STATUS "OpenGL backend enabled - will link OpenGL libraries")
    
    # Define preprocessor macro for GL backend
    add_compile_definitions(VGLITE_USE_GL_BACKEND)
else()
    message(STATUS "Software backend enabled - no OpenGL linking")
endif()

# Get absolute paths
get_filename_component(VGLITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

message(STATUS "VGLITE_ROOT: ${VGLITE_ROOT}")

# ============================================================================
# Build examples (requires GLFW + OpenGL)
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
    message(STATUS "Including examples...")
    add_subdirectory(examples)
endif()

# ============================================================================
# Build tests
# ============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
    message(STATUS "Including tests...")
    add_subdirectory(tests)
endif()