# VGLite ThorVG Android Demo
cmake_minimum_required(VERSION 3.18.1)
project(vglite_android_demo C CXX)

# ============================================================================
# Platform Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Paths
# ============================================================================
# From: examples/demos/clear_demo/android/app/src/main/cpp
# To:   vglite_thorvg (9 levels up)
set(VGLITE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../..)
set(THORVG_ROOT ${VGLITE_ROOT}/libs/thorvg-1.0.2)
set(THORVG_SRC_DIR ${THORVG_ROOT}/src)
set(THORVG_INC_DIR ${THORVG_ROOT}/inc)

# ============================================================================
# ThorVG Core Sources (from src/renderer/)
# ============================================================================
set(THORVG_SOURCES
    ${THORVG_SRC_DIR}/renderer/tvgAccessor.cpp
    ${THORVG_SRC_DIR}/renderer/tvgAnimation.cpp
    ${THORVG_SRC_DIR}/renderer/tvgCanvas.cpp
    ${THORVG_SRC_DIR}/renderer/tvgFill.cpp
    ${THORVG_SRC_DIR}/renderer/tvgInitializer.cpp
    ${THORVG_SRC_DIR}/renderer/tvgLoader.cpp
    ${THORVG_SRC_DIR}/renderer/tvgPaint.cpp
    ${THORVG_SRC_DIR}/renderer/tvgPicture.cpp
    ${THORVG_SRC_DIR}/renderer/tvgRender.cpp
    ${THORVG_SRC_DIR}/renderer/tvgSaver.cpp
    ${THORVG_SRC_DIR}/renderer/tvgScene.cpp
    ${THORVG_SRC_DIR}/renderer/tvgShape.cpp
    ${THORVG_SRC_DIR}/renderer/tvgTaskScheduler.cpp
    ${THORVG_SRC_DIR}/renderer/tvgText.cpp
)

# ============================================================================
# ThorVG Common Sources
# ============================================================================
set(THORVG_COMMON_SOURCES
    ${THORVG_SRC_DIR}/common/tvgColor.cpp
    ${THORVG_SRC_DIR}/common/tvgCompressor.cpp
    ${THORVG_SRC_DIR}/common/tvgMath.cpp
    ${THORVG_SRC_DIR}/common/tvgStr.cpp
)

# ============================================================================
# ThorVG SW Engine Sources
# ============================================================================
set(THORVG_SW_SOURCES
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwFill.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwImage.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwMath.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwMemPool.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwPostEffect.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwRaster.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwRenderer.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwRle.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwShape.cpp
    ${THORVG_SRC_DIR}/renderer/sw_engine/tvgSwStroke.cpp
)

# ============================================================================
# ThorVG GL Engine Sources (for GPU rendering)
# ============================================================================
set(THORVG_GL_SOURCES
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGl.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlRenderer.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlRenderTask.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlRenderPass.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlRenderTarget.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlProgram.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlShader.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlShaderSrc.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlGpuBuffer.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlGeometry.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlTessellator.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlSolidBatch.cpp
    ${THORVG_SRC_DIR}/renderer/gl_engine/tvgGlEffect.cpp
)

# ============================================================================
# ThorVG SVG Loader Sources
# ============================================================================
set(THORVG_SVG_SOURCES
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgCssStyle.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgLoader.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgPath.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgSceneBuilder.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgUtil.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgXmlParser.cpp
)

# ============================================================================
# ThorVG Lottie Loader Sources
# ============================================================================
set(THORVG_LOTTIE_SOURCES
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieAnimation.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieBuilder.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieExpressions.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieInterpolator.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieLoader.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieModel.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieModifier.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieParser.cpp
    ${THORVG_SRC_DIR}/loaders/lottie/tvgLottieParserHandler.cpp
)

# ============================================================================
# ThorVG PNG Loader Sources (includes lodepng)
# ============================================================================
set(THORVG_PNG_SOURCES
    ${THORVG_SRC_DIR}/loaders/png/tvgPngLoader.cpp
    ${THORVG_SRC_DIR}/loaders/png/tvgLodePng.cpp
)

# ============================================================================
# ThorVG JPG Loader Sources (includes jpgd)
# ============================================================================
set(THORVG_JPG_SOURCES
    ${THORVG_SRC_DIR}/loaders/jpg/tvgJpgLoader.cpp
    ${THORVG_SRC_DIR}/loaders/jpg/tvgJpgd.cpp
)

# ============================================================================
# ThorVG Raw Loader Sources
# ============================================================================
set(THORVG_RAW_SOURCES
    ${THORVG_SRC_DIR}/loaders/raw/tvgRawLoader.cpp
)

# ============================================================================
# ThorVG PNG Saver Sources (no PNG saver in ThorVG 1.0.2, only GIF)
# ============================================================================
# Note: PNG saver not available in this ThorVG version
# set(THORVG_SAVER_SOURCES
#     ${THORVG_SRC_DIR}/savers/png/tvgPngSaver.cpp
# )

# ============================================================================
# VGLite ThorVG Sources
# ============================================================================
set(VGLITE_SOURCES
    ${VGLITE_ROOT}/vg_lite_tvg.cpp
    ${VGLITE_ROOT}/vg_lite_matrix.c
)

# ============================================================================
# Demo Sources
# ============================================================================
set(DEMO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# ============================================================================
# Include Directories
# ============================================================================
include_directories(
    ${VGLITE_ROOT}
    ${THORVG_INC_DIR}
    ${THORVG_SRC_DIR}
    ${THORVG_SRC_DIR}/common
    ${THORVG_SRC_DIR}/renderer
    ${THORVG_SRC_DIR}/renderer/sw_engine
    ${THORVG_SRC_DIR}/renderer/gl_engine
    ${THORVG_SRC_DIR}/loaders
    ${THORVG_SRC_DIR}/loaders/svg
    ${THORVG_SRC_DIR}/loaders/lottie
    ${THORVG_SRC_DIR}/loaders/png
    ${THORVG_SRC_DIR}/loaders/jpg
    ${THORVG_SRC_DIR}/loaders/raw
    ${THORVG_SRC_DIR}/savers
)

# ============================================================================
# Libraries
# ============================================================================
# Find Android system libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(gles-lib GLESv3)

# Create native library (include native_app_glue source directly)
add_library(vglite_demo SHARED
    ${DEMO_SOURCES}
    ${VGLITE_SOURCES}
    ${THORVG_SOURCES}
    ${THORVG_COMMON_SOURCES}
    ${THORVG_SW_SOURCES}
    ${THORVG_GL_SOURCES}
    ${THORVG_SVG_SOURCES}
    ${THORVG_LOTTIE_SOURCES}
    ${THORVG_PNG_SOURCES}
    ${THORVG_JPG_SOURCES}
    ${THORVG_RAW_SOURCES}
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

# Compile definitions for the library
target_compile_definitions(vglite_demo PRIVATE
    THORVG_GL_TARGET_GLES=1
    TVG_ENABLE_GL_ENGINE=1
    __ANDROID__
)

# Include native_app_glue header (NDK r27+ uses this path)
target_include_directories(vglite_demo PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
)

# Link libraries
target_link_libraries(vglite_demo
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${gles-lib}
)
