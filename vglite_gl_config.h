/**
 * @file vglite_gl_config.h
 * @brief GL backend configuration for VGLite ThorVG
 * 
 * This header provides compile-time configuration for switching between
 * SW (software) and GL (OpenGL) backends, with platform detection.
 */

#ifndef VGLITE_GL_CONFIG_H
#define VGLITE_GL_CONFIG_H

/* ============================================================================
 * Platform Detection
 * ============================================================================ */
#if defined(__ANDROID__)
    #define VGLITE_PLATFORM_ANDROID 1
    #define VGLITE_PLATFORM_WINDOWS 0
#elif defined(_WIN32)
    #define VGLITE_PLATFORM_ANDROID 0
    #define VGLITE_PLATFORM_WINDOWS 1
#else
    #define VGLITE_PLATFORM_ANDROID 0
    #define VGLITE_PLATFORM_WINDOWS 0
#endif

/* ============================================================================
 * GL Backend Selection
 * ============================================================================
 * 0 = SW backend (default, CPU rendering via SwCanvas)
 * 1 = GL backend (GPU rendering via GlCanvas)
 * 
 * Android: GL backend is enabled by default
 * Windows: SW backend is default, enable via VGLITE_USE_GL_BACKEND cmake option
 */
#ifndef VGLITE_USE_GL_BACKEND
    #if VGLITE_PLATFORM_ANDROID
        /* Android uses GL backend by default (GPU rendering) */
        #define VGLITE_USE_GL_BACKEND 1
    #else
        /* Windows/Linux default to SW backend */
        #define VGLITE_USE_GL_BACKEND 0
    #endif
#endif

/* ============================================================================
 * ThorVG GL Target Configuration
 * ============================================================================ */
#if VGLITE_PLATFORM_ANDROID
    /* Android uses OpenGL ES */
    #define THORVG_GL_TARGET_GLES 1
#endif

/* Conditional canvas type */
#if VGLITE_USE_GL_BACKEND
    #define TVG_GL_CANVAS tvg::GlCanvas
#else
    #define TVG_GL_CANVAS tvg::SwCanvas
#endif

/* Helper macros for conditional compilation */
#if VGLITE_USE_GL_BACKEND
    #define VGLITE_IF_GL(code) code
    #define VGLITE_IF_SW(code)
#else
    #define VGLITE_IF_GL(code)
    #define VGLITE_IF_SW(code) code
#endif

/* Platform-specific helper macros */
#if VGLITE_PLATFORM_ANDROID
    #define VGLITE_IF_ANDROID(code) code
    #define VGLITE_IF_WINDOWS(code)
#elif VGLITE_PLATFORM_WINDOWS
    #define VGLITE_IF_ANDROID(code)
    #define VGLITE_IF_WINDOWS(code) code
#else
    #define VGLITE_IF_ANDROID(code)
    #define VGLITE_IF_WINDOWS(code)
#endif

#endif /* VGLITE_GL_CONFIG_H */
