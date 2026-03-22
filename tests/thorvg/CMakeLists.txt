# ThorVG 1.0.2 Library Build Configuration
# Support for dual ThorVG builds (thorvg_gl + thorvg_sw)
cmake_minimum_required(VERSION 3.14)

project(thorvg CXX)

# ThorVG 1.0.2 source directory
set(THORVG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/thorvg-1.0.2)
set(THORVG_SRC_DIR ${THORVG_ROOT}/src)
set(THORVG_INC_DIR ${THORVG_ROOT}/inc)

# ThorVG core source files (from src/renderer/)
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

# ThorVG common source files
set(THORVG_COMMON_SOURCES
    ${THORVG_SRC_DIR}/common/tvgColor.cpp
    ${THORVG_SRC_DIR}/common/tvgCompressor.cpp
    ${THORVG_SRC_DIR}/common/tvgMath.cpp
    ${THORVG_SRC_DIR}/common/tvgStr.cpp
)

# ThorVG Software Renderer sources (from src/renderer/sw_engine/)
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

# ThorVG SVG Loader sources (from src/loaders/svg/)
set(THORVG_SVG_SOURCES
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgCssStyle.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgLoader.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgPath.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgSceneBuilder.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgSvgUtil.cpp
    ${THORVG_SRC_DIR}/loaders/svg/tvgXmlParser.cpp
)

# ThorVG Lottie Loader sources (from src/loaders/lottie/)
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

# ThorVG Raw Loader sources (from src/loaders/raw/)
set(THORVG_RAW_SOURCES
    ${THORVG_SRC_DIR}/loaders/raw/tvgRawLoader.cpp
)

# ThorVG GL Engine sources (from src/renderer/gl_engine/)
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
# Dual ThorVG Build Mode
# ============================================================================
if(VGLITE_BUILD_DUAL_THORVG)
    message(STATUS "Building ThorVG in DUAL mode (thorvg_gl + thorvg_sw)")
    
    # ------------------------------------------------------------------------
    # thorvg_gl - includes GL engine
    # ------------------------------------------------------------------------
    add_library(thorvg_gl STATIC
        ${THORVG_SOURCES}
        ${THORVG_COMMON_SOURCES}
        ${THORVG_SW_SOURCES}
        ${THORVG_GL_SOURCES}
        ${THORVG_SVG_SOURCES}
        ${THORVG_LOTTIE_SOURCES}
        ${THORVG_RAW_SOURCES}
    )
    
    target_include_directories(thorvg_gl PUBLIC
        ${THORVG_INC_DIR}
        ${THORVG_SRC_DIR}
        ${THORVG_SRC_DIR}/renderer
        ${THORVG_SRC_DIR}/renderer/sw_engine
        ${THORVG_SRC_DIR}/renderer/gl_engine
        ${THORVG_SRC_DIR}/loaders/svg
        ${THORVG_SRC_DIR}/loaders/lottie
        ${THORVG_SRC_DIR}/loaders/raw
        ${THORVG_SRC_DIR}/common
        ${THORVG_SRC_DIR}/loaders/lottie/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
    )
    
    target_compile_features(thorvg_gl PUBLIC cxx_std_17)
    
    target_compile_definitions(thorvg_gl PRIVATE
        TVG_ENABLE_SVG_LOADER
        TVG_ENABLE_LOTTIE_LOADER
        TVG_ENABLE_SW_ENGINE
        TVG_ENABLE_GL_ENGINE
        VGLITE_USE_GL_BACKEND
        THORVG_GL_TARGET_GL
        TVG_STATIC
        __STDC_FORMAT_MACROS
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
        NOMINMAX
    )
    
    # ------------------------------------------------------------------------
    # thorvg_sw - software only (no GL engine)
    # ------------------------------------------------------------------------
    add_library(thorvg_sw STATIC
        ${THORVG_SOURCES}
        ${THORVG_COMMON_SOURCES}
        ${THORVG_SW_SOURCES}
        ${THORVG_SVG_SOURCES}
        ${THORVG_LOTTIE_SOURCES}
        ${THORVG_RAW_SOURCES}
    )
    
    target_include_directories(thorvg_sw PUBLIC
        ${THORVG_INC_DIR}
        ${THORVG_SRC_DIR}
        ${THORVG_SRC_DIR}/renderer
        ${THORVG_SRC_DIR}/renderer/sw_engine
        ${THORVG_SRC_DIR}/loaders/svg
        ${THORVG_SRC_DIR}/loaders/lottie
        ${THORVG_SRC_DIR}/loaders/raw
        ${THORVG_SRC_DIR}/common
        ${THORVG_SRC_DIR}/loaders/lottie/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
    )
    
    target_compile_features(thorvg_sw PUBLIC cxx_std_17)
    
    target_compile_definitions(thorvg_sw PRIVATE
        TVG_ENABLE_SVG_LOADER
        TVG_ENABLE_LOTTIE_LOADER
        TVG_ENABLE_SW_ENGINE
        TVG_STATIC
        __STDC_FORMAT_MACROS
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
        NOMINMAX
    )
    
    # Export both libraries
    set(THORVG_GL_LIBRARY thorvg_gl PARENT_SCOPE)
    set(THORVG_SW_LIBRARY thorvg_sw PARENT_SCOPE)
    
# ============================================================================
# Single ThorVG Build Mode (backward compatible)
# ============================================================================
else()
    message(STATUS "Building ThorVG in SINGLE mode")
    
    # Create ThorVG static library
    if(VGLITE_USE_GL_BACKEND)
        add_library(thorvg STATIC
            ${THORVG_SOURCES}
            ${THORVG_COMMON_SOURCES}
            ${THORVG_SW_SOURCES}
            ${THORVG_GL_SOURCES}
            ${THORVG_SVG_SOURCES}
            ${THORVG_LOTTIE_SOURCES}
            ${THORVG_RAW_SOURCES}
        )
    else()
        add_library(thorvg STATIC
            ${THORVG_SOURCES}
            ${THORVG_COMMON_SOURCES}
            ${THORVG_SW_SOURCES}
            ${THORVG_SVG_SOURCES}
            ${THORVG_LOTTIE_SOURCES}
            ${THORVG_RAW_SOURCES}
        )
    endif()
    
    target_include_directories(thorvg PUBLIC
        ${THORVG_INC_DIR}
        ${THORVG_SRC_DIR}
        ${THORVG_SRC_DIR}/renderer
        ${THORVG_SRC_DIR}/renderer/sw_engine
        ${THORVG_SRC_DIR}/renderer/gl_engine
        ${THORVG_SRC_DIR}/loaders/svg
        ${THORVG_SRC_DIR}/loaders/lottie
        ${THORVG_SRC_DIR}/loaders/raw
        ${THORVG_SRC_DIR}/common
        ${THORVG_SRC_DIR}/loaders/lottie/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
    )
    
    target_compile_features(thorvg PUBLIC cxx_std_17)
    
    # ThorVG compile definitions
    target_compile_definitions(thorvg PRIVATE
        TVG_ENABLE_SVG_LOADER
        TVG_ENABLE_LOTTIE_LOADER
        TVG_ENABLE_SW_ENGINE
        TVG_STATIC
        __STDC_FORMAT_MACROS
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
        NOMINMAX
    )
    
    # Enable GL backend if requested
    if(VGLITE_USE_GL_BACKEND)
        target_compile_definitions(thorvg PRIVATE
            TVG_ENABLE_GL_ENGINE
        )
        message(STATUS "ThorVG GL engine enabled")
    endif()
    
    # Export library for parent CMakeLists.txt
    set(THORVG_LIBRARY thorvg PARENT_SCOPE)
endif()
