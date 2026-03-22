/**
 * @file main.cpp
 * @brief Android NativeActivity entry point for VGLite clear demo
 * 
 * This demo implements a 5-color cycling clear demo using vg_lite API
 * with ThorVG GL backend on Android EGL.
 */

#include <android_native_app_glue.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <unistd.h>
#include <cstring>

// VGLite headers
extern "C" {
#include "vg_lite.h"
}

// GL backend configuration
#include "vglite_gl_config.h"

// ============================================================================
// Logging
// ============================================================================
#define LOG_TAG "VGLiteDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ============================================================================
// Configuration
// ============================================================================
static const int COLOR_SWITCH_INTERVAL_MS = 1000;

// Colors in ABGR format (matches ThorVG GL backend ColorSpace::ABGR8888S)
static const vg_lite_color_t COLORS[] = {
    0xFF0000FF,  // Red    (ABGR: A=FF, B=00, G=00, R=FF)
    0xFF00FF00,  // Green  (ABGR: A=FF, B=00, G=FF, R=00)
    0xFFFF0000,  // Blue   (ABGR: A=FF, B=FF, G=00, R=00)
    0xFF00FFFF,  // Yellow (ABGR: A=FF, B=00, G=FF, R=FF)
    0xFFFFFF00   // Cyan   (ABGR: A=FF, B=FF, G=FF, R=00)
};
static const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

// ============================================================================
// EGL State
// ============================================================================
struct EglContext {
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    int width = 0;
    int height = 0;
};

static EglContext g_egl;

// ============================================================================
// EGL Initialization
// ============================================================================
static bool init_egl(android_app* app) {
    // Get EGL display
    g_egl.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_egl.display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(g_egl.display, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL version: %d.%d", major, minor);
    
    // Choose config for OpenGL ES 3.0
    EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 0,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint num_configs;
    if (!eglChooseConfig(g_egl.display, config_attribs, &config, 1, &num_configs)) {
        LOGE("eglChooseConfig failed");
        return false;
    }
    
    // Create EGL surface
    g_egl.surface = eglCreateWindowSurface(g_egl.display, config, app->window, nullptr);
    if (g_egl.surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed: 0x%x", eglGetError());
        return false;
    }
    
    // Create EGL context for OpenGL ES 3.0
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    g_egl.context = eglCreateContext(g_egl.display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_egl.context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed: 0x%x", eglGetError());
        return false;
    }
    
    // Make context current
    if (!eglMakeCurrent(g_egl.display, g_egl.surface, g_egl.surface, g_egl.context)) {
        LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }
    
    // Get surface dimensions
    eglQuerySurface(g_egl.display, g_egl.surface, EGL_WIDTH, &g_egl.width);
    eglQuerySurface(g_egl.display, g_egl.surface, EGL_HEIGHT, &g_egl.height);
    
    LOGI("EGL init success: %dx%d", g_egl.width, g_egl.height);
    return true;
}

// ============================================================================
// EGL Termination
// ============================================================================
static void term_egl() {
    if (g_egl.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (g_egl.context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_egl.display, g_egl.context);
        }
        if (g_egl.surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_egl.display, g_egl.surface);
        }
        eglTerminate(g_egl.display);
    }
    g_egl.display = EGL_NO_DISPLAY;
    g_egl.surface = EGL_NO_SURFACE;
    g_egl.context = EGL_NO_CONTEXT;
    LOGI("EGL terminated");
}

// ============================================================================
// VGLite Initialization
// ============================================================================
static bool init_vglite() {
    // Initialize vg_lite with canvas dimensions
    vg_lite_error_t error = vg_lite_init(g_egl.width, g_egl.height);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_init failed: %d", error);
        return false;
    }
    
    // Get current EGL context handles
    EGLDisplay display = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    
    // Set GL context for vg_lite
    error = vg_lite_set_gl_context(
        (void*)display,
        (void*)surface,
        (void*)context,
        0  // fbo_id = 0 for default framebuffer
    );
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_set_gl_context failed: %d", error);
        return false;
    }
    
    LOGI("VGLITE init success: %dx%d", g_egl.width, g_egl.height);
    return true;
}

// ============================================================================
// VGLite Termination
// ============================================================================
static void term_vglite() {
    vg_lite_close();
    LOGI("VGLITE terminated");
}

// ============================================================================
// Render Loop
// ============================================================================
static void render_frame(int color_index) {
    // Create dummy buffer (GL backend doesn't use memory)
    vg_lite_buffer_t buffer = {0};
    buffer.width = g_egl.width;
    buffer.height = g_egl.height;
    buffer.format = VG_LITE_BGRA8888;
    
    // Clear with current color
    vg_lite_color_t color = COLORS[color_index];
    vg_lite_error_t error = vg_lite_clear(&buffer, nullptr, color);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_clear failed: %d", error);
        return;
    }
    
    // Finish rendering (draws to GL FBO)
    error = vg_lite_finish();
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_finish failed: %d", error);
        return;
    }
    
    // Swap buffers to display
    eglSwapBuffers(g_egl.display, g_egl.surface);
}

// ============================================================================
// Main Entry Point
// ============================================================================
void android_main(android_app* app) {
    LOGI("=== VGLite Clear Demo (Android) ===");
    LOGI("Backend: OpenGL ES (ThorVG GL Engine)");
    LOGI("Window: %p", app->window);
    LOGI("Colors: %d, Interval: %dms", NUM_COLORS, COLOR_SWITCH_INTERVAL_MS);
    
    // Initialize EGL
    if (!init_egl(app)) {
        LOGE("Failed to initialize EGL");
        return;
    }
    
    // Initialize VGLite
    if (!init_vglite()) {
        LOGE("Failed to initialize VGLite");
        term_egl();
        return;
    }
    
    // Render loop
    int color_index = 0;
    int cycle_count = 0;
    bool running = true;
    
    while (running) {
        // Process Android events using ALooper_pollOnce (pollAll is deprecated)
        int ident;
        int events;
        android_poll_source* source;
        
        // Process all pending events
        while ((ident = ALooper_pollOnce(0, nullptr, &events, (void**)&source)) >= 0) {
            if (ident == ALOOPER_POLL_TIMEOUT) {
                break;  // No more events
            }
            if (source != nullptr) {
                source->process(app, source);
            }
            
            if (app->destroyRequested != 0) {
                running = false;
                break;
            }
        }
        
        if (!running) break;
        
        // Render current color
        const char* color_name = (color_index == 0) ? "Red" :
                                 (color_index == 1) ? "Green" :
                                 (color_index == 2) ? "Blue" :
                                 (color_index == 3) ? "Yellow" : "Cyan";
        
        LOGI("Cycle %d: %s (0x%08X)", cycle_count + 1, color_name, COLORS[color_index]);
        render_frame(color_index);
        
        // Advance to next color
        color_index = (color_index + 1) % NUM_COLORS;
        cycle_count++;
        
        // Sleep for color switch interval
        usleep(COLOR_SWITCH_INTERVAL_MS * 1000);
    }
    
    // Cleanup
    LOGI("Cleaning up...");
    term_vglite();
    term_egl();
    LOGI("Demo finished");
}
