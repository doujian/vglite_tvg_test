cmake_minimum_required(VERSION 3.14)

project(vglite_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Force shared C runtime for Visual Studio compatibility
# set(gtest_force_shared_crt ON CACHE bool "" CACHE)
set(CMAKE_CXX_SCAN "")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force shared C runtime for Visual Studio compatibility
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Get absolute paths
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)
get_filename_component(VGLITE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)

message(STATUS "PROJECT_ROOT: ${PROJECT_ROOT}")
message(STATUS "VGLITE_ROOT: ${VGLITE_ROOT}")

# Build utility libraries (zlib, libpng, vg_lite_util)
add_subdirectory(utils)

# Build ThorVG library
add_subdirectory(thorvg)

# Embedded GoogleTest source (must be before enable_testing)
add_subdirectory(third_party/googletest)

enable_testing()
include(CTest)

# Test executable
add_executable(vglite_tests
    src/main.cpp
    src/build_verification_test.cpp
    src/image_compare.cpp
    src/samples/clear_test.cpp
    src/samples/arc_path_test.cpp
    src/samples/blend_mode_test.cpp
    src/samples/colorkey_test.cpp
    src/samples/linear_grad_test.cpp
    src/samples/radial_grad_test.cpp
    src/samples/rotate_test.cpp
    src/samples/scale_test.cpp
    src/samples/vector_test.cpp
    src/samples/stroke_test.cpp
    src/samples/pattern_fill_test.cpp
    src/samples/mask_test.cpp
    src/samples/scissor_test.cpp
    src/samples/stencil_test.cpp
    src/samples/pixels_align_test.cpp
    src/samples/global_alpha_test.cpp
    src/samples/gamma_test.cpp
    src/samples/mirror_test.cpp
    src/samples/color_transformation_test.cpp
    src/samples/dither_test.cpp
    src/samples/gaussian_blur_test.cpp
    src/samples/multi_draw_test.cpp
    src/samples/img_index_test.cpp
    src/samples/img_a4_test.cpp
    src/samples/img_a8_test.cpp
    src/samples/blend_lvgl_test.cpp
    src/samples/blend_premultiply_test.cpp
    src/samples/pixel_matrix_test.cpp
    src/samples/recolor_test.cpp
    src/samples/double_image_test.cpp
    src/samples/format_24bit_test.cpp
    src/samples/new_blend_mode_test.cpp
    src/unit_test/clear_unit_test.cpp
    src/unit_test/combination_test.cpp
    src/unit_test/draw_image_test.cpp
    src/unit_test/tessellation_test.cpp
    src/unit_test1/api_test.cpp
    src/unit_test1/gradient_test.cpp
    src/unit_test1/matrix_test.cpp
    src/unit_test1/memory_test.cpp
    src/unit_test1/pattern_test.cpp
)

# Include directories
target_include_directories(vglite_tests PRIVATE
    ${VGLITE_ROOT}                            # Parent vglite_thorvg directory (for vglite_config.h)
    ${VGLITE_ROOT}/libs/thorvg-1.0.2/inc      # ThorVG 1.0.2 library headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/        # vg_lite_util header
)

# Link against parent vglite_thorvg sources
target_sources(vglite_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../vg_lite_tvg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../vg_lite_matrix.c
)

# Define TVG_STATIC for static linking to ThorVG
target_compile_definitions(vglite_tests PRIVATE
    TVG_STATIC
)

# Link GoogleTest, ThorVG and utility libraries
target_link_libraries(vglite_tests PRIVATE
    gtest
    gtest_main
    vg_lite_util
    thorvg
)

# Register tests with CTest
# Set working directory to tests/ so resources/ paths work correctly
add_test(NAME vglite_tests COMMAND vglite_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
