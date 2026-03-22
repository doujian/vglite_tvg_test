/**
 * @file vglite_gl_backend.h
 * @brief VGLite GL Backend Support
 * 
 * This header provides GL-specific implementations for GL backend support.
    The APIs allow CPU format conversion between SW and GL.
 */

#ifndef VGLITE_GL_BACKEND_H
#define VGLITE_GL_BACKEND_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

#if VGLITE_USE_GL_BACKEND

/**
 * @brief Set external GL context for VGLite
 * 
 * @param display The EGLDisplay or WGL HDC
 * @param surface The EGLSurface or GLFWwindow
 * @param context The EGLContext or GLFW OpenGL context handle
 * @param fbo_id The FBO ID (0 for default framebuffer)
 * @param width The Surface width
 * @param height The Surface height
 * @return vg_lite_error_t error code
 */
vg_lite_error_t vg_lite_set_gl_context(
    void* display,
    void* surface,
    void* context,
    int32_t fbo_id,
    uint32_t width,
    uint32_t height
);

/**
 * @brief Get current GL FBO ID
 * @return The FBO ID currently set for GL context
 */
int32_t vg_lite_get_gl_fbo_id(void);

#endif // VGLITE_USE_GL_BACKEND

#endif // VGLITE_GL_BACKEND_H
