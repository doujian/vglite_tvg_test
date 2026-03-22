/**
 * @file vglite_gl_config.h
 * @brief GL backend configuration for VGLite ThorVG
 * 
 * This header provides compile-time configuration for switching between
 * SW (software) and GL (OpenGL) backends.
 */

#ifndef VGLITE_GL_CONFIG_H
#define VGLITE_GL_CONFIG_H

/* GL Backend Selection
 * 0 = SW backend (default, CPU rendering via SwCanvas)
 * 1 = GL backend (GPU rendering via GlCanvas)
 */
#ifndef VGLITE_USE_GL_BACKEND
#define VGLITE_USE_GL_BACKEND 0
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

#endif /* VGLITE_GL_CONFIG_H */
