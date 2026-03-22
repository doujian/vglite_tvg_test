# tests/utils/CMakeLists.txt
# Build configuration for VGLite utility library with libpng and zlib dependencies
# Windows/Visual Studio compatible

cmake_minimum_required(VERSION 3.14)

# ============================================================================
# zlib static library (v1.2.7)
# ============================================================================
add_library(zlib_static STATIC
    zlib/adler32.c
    zlib/compress.c
    zlib/crc32.c
    zlib/deflate.c
    zlib/gzclose.c
    zlib/gzlib.c
    zlib/gzread.c
    zlib/gzwrite.c
    zlib/infback.c
    zlib/inffast.c
    zlib/inflate.c
    zlib/inftrees.c
    zlib/trees.c
    zlib/uncompr.c
    zlib/zutil.c
)

target_include_directories(zlib_static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
)

# Platform-specific definitions for zlib
if(WIN32)
    target_compile_definitions(zlib_static PRIVATE
        ZLIB_DLL
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
    )
endif()

# ============================================================================
# libpng static library (v1.6.1)
# ============================================================================
add_library(png_static STATIC
    libpng/png.c
    libpng/pngerror.c
    libpng/pngget.c
    libpng/pngmem.c
    libpng/pngpread.c
    libpng/pngread.c
    libpng/pngrio.c
    libpng/pngrtran.c
    libpng/pngrutil.c
    libpng/pngset.c
    libpng/pngtrans.c
    libpng/pngwio.c
    libpng/pngwrite.c
    libpng/pngwtran.c
    libpng/pngwutil.c
)

target_include_directories(png_static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libpng
)

target_link_libraries(png_static PUBLIC
    zlib_static
)

# Platform-specific definitions for libpng
if(WIN32)
    target_compile_definitions(png_static PRIVATE
        PNG_DLL
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
    )
endif()

# ============================================================================
# vg_lite_util static library
# ============================================================================
add_library(vg_lite_util STATIC
    vg_lite_util.c
)

target_include_directories(vg_lite_util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libpng
    ${CMAKE_CURRENT_SOURCE_DIR}/zlib
    ${CMAKE_CURRENT_SOURCE_DIR}/../..   # For vg_lite.h
)

target_link_libraries(vg_lite_util PUBLIC
    png_static
    zlib_static
)

# Platform-specific definitions for vg_lite_util
if(WIN32)
    target_compile_definitions(vg_lite_util PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
    )
endif()

# Optional: Enable framebuffer support on Linux
if(NOT WIN32)
    option(HAVE_FBDEV "Enable framebuffer device support" OFF)
    if(HAVE_FBDEV)
        target_compile_definitions(vg_lite_util PRIVATE HAVE_FBDEV)
    endif()
endif()

# ============================================================================
# Export libraries for parent CMakeLists.txt
# ============================================================================
set(VG_LITE_UTIL_LIBRARIES
    vg_lite_util
    png_static
    zlib_static
    PARENT_SCOPE
)
