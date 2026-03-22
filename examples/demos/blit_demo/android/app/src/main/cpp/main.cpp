/**
 * @file main.cpp
 * @brief Android NativeActivity entry point for VGLite blit demo
 * 
 * This demo implements a moving colored square using vg_lite_blit API
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
#define LOG_TAG "VGLiteBlitDemo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ============================================================================
// Configuration
// ============================================================================
static const int MOVE_INTERVAL_MS = 1000;
static const int SQUARE_SIZE = 100;
static const int PADDING = 50;

// Colors in ABGR format
static const vg_lite_color_t COLORS[] = {
    0xFF0000FF,  // Red
    0xFF00FF00,  // Green
    0xFFFF0000,  // Blue
    0xFF00FFFF,  // Yellow
    0xFFFFFF00   // Cyan
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
// Global state
// ============================================================================
static vg_lite_buffer_t g_source_buffer = {0};

// ============================================================================
// EGL Initialization
// ============================================================================
static bool init_egl(android_app* app) {
    g_egl.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_egl.display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(g_egl.display, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL version: %d.%d", major, minor);
    
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
    
    g_egl.surface = eglCreateWindowSurface(g_egl.display, config, app->window, nullptr);
    if (g_egl.surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed: 0x%x", eglGetError());
        return false;
    }
    
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    g_egl.context = eglCreateContext(g_egl.display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_egl.context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed: 0x%x", eglGetError());
        return false;
    }
    
    if (!eglMakeCurrent(g_egl.display, g_egl.surface, g_egl.surface, g_egl.context)) {
        LOGE("eglMakeCurrent failed: 0x%x", eglGetError());
        return false;
    }
    
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
// Create source buffer
// ============================================================================
static void create_source_buffer(int color_index) {
    memset(&g_source_buffer, 0, sizeof(g_source_buffer));
    g_source_buffer.width = SQUARE_SIZE;
    g_source_buffer.height = SQUARE_SIZE;
    g_source_buffer.format = VG_LITE_BGRA8888;
    
    vg_lite_error_t error = vg_lite_allocate(&g_source_buffer);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_allocate failed: %d", error);
        return;
    }
    
    // Fill with color
    vg_lite_color_t color = COLORS[color_index % NUM_COLORS];
    uint8_t* pixels = (uint8_t*)g_source_buffer.memory;
    
    for (int y = 0; y < SQUARE_SIZE; y++) {
        for (int x = 0; x < SQUARE_SIZE; x++) {
            int idx = (y * SQUARE_SIZE + x) * 4;
            pixels[idx + 0] = (color >> 0) & 0xFF;   // B
            pixels[idx + 1] = (color >> 8) & 0xFF;   // G
            pixels[idx + 2] = (color >> 16) & 0xFF;  // R
            pixels[idx + 3] = (color >> 24) & 0xFF;  // A
        }
    }
}

// ============================================================================
// Update source color
// ============================================================================
static void update_source_color(int color_index) {
    vg_lite_color_t color = COLORS[color_index % NUM_COLORS];
    uint8_t* pixels = (uint8_t*)g_source_buffer.memory;
    
    for (int y = 0; y < SQUARE_SIZE; y++) {
        for (int x = 0; x < SQUARE_SIZE; x++) {
            int idx = (y * SQUARE_SIZE + x) * 4;
            pixels[idx + 0] = (color >> 0) & 0xFF;
            pixels[idx + 1] = (color >> 8) & 0xFF;
            pixels[idx + 2] = (color >> 16) & 0xFF;
            pixels[idx + 3] = (color >> 24) & 0xFF;
        }
    }
}

// ============================================================================
// VGLite Initialization
// ============================================================================
static bool init_vglite() {
    vg_lite_error_t error = vg_lite_init(g_egl.width, g_egl.height);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_init failed: %d", error);
        return false;
    }
    
    EGLDisplay display = eglGetCurrentDisplay();
    EGLContext context = eglGetCurrentContext();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    
    error = vg_lite_set_gl_context(
        (void*)display,
        (void*)surface,
        (void*)context,
        0
    );
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_set_gl_context failed: %d", error);
        return false;
    }
    
    create_source_buffer(0);
    LOGI("VGLite init success");
    return true;
}

// ============================================================================
// VGLite Termination
// ============================================================================
static void term_vglite() {
    vg_lite_free(&g_source_buffer);
    vg_lite_close();
    LOGI("VGLite terminated");
}

// ============================================================================
// Render Loop
// ============================================================================
static void render_frame(int corner_index, int color_index) {
    // Calculate corner position
    float x = 0, y = 0;
    switch (corner_index) {
        case 0: // Top-left
            x = PADDING;
            y = PADDING;
            break;
        case 1: // Top-right
            x = g_egl.width - SQUARE_SIZE - PADDING;
            y = PADDING;
            break;
        case 2: // Bottom-right
            x = g_egl.width - SQUARE_SIZE - PADDING;
            y = g_egl.height - SQUARE_SIZE - PADDING;
            break;
        case 3: // Bottom-left
            x = PADDING;
            y = g_egl.height - SQUARE_SIZE - PADDING;
            break;
    }
    
    // Update color
    update_source_color(color_index);
    
    // Create target buffer (dummy for GL)
    vg_lite_buffer_t target = {0};
    target.width = g_egl.width;
    target.height = g_egl.height;
    target.format = VG_LITE_BGRA8888;
    
    // Clear to black
    vg_lite_error_t error = vg_lite_clear(&target, nullptr, 0xFF000000);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_clear failed: %d", error);
        return;
    }
    
    // Setup matrix
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(x, y, &matrix);
    
    // Blit the square
    error = vg_lite_blit(&target, &g_source_buffer, &matrix,
                        VG_LITE_BLEND_SRC_OVER, 0xFFFFFFFF, VG_LITE_FILTER_POINT);
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_blit failed: %d", error);
        return;
    }
    
    // Finish rendering
    error = vg_lite_finish();
    if (error != VG_LITE_SUCCESS) {
        LOGE("vg_lite_finish failed: %d", error);
        return;
    }
    
    // Swap buffers
    eglSwapBuffers(g_egl.display, g_egl.surface);
}

// ============================================================================
// Global state
// ============================================================================
static bool g_window_ready = false;
static bool g_egl_initialized = false;
static bool g_vglite_initialized = false;
static int g_corner_index = 0;
static int g_color_index = 0;
static int g_cycle_count = 0;

// ============================================================================
// Command handler
// ============================================================================
static void handle_app_cmd(android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW: window=%p", app->window);
            if (app->window != nullptr) {
                g_window_ready = true;
                
                if (!g_egl_initialized) {
                    if (init_egl(app)) {
                        g_egl_initialized = true;
                        LOGI("EGL initialized");
                        
                        if (init_vglite()) {
                            g_vglite_initialized = true;
                            LOGI("VGLite initialized");
                        } else {
                            LOGE("VGLite init failed");
                        }
                    } else {
                        LOGE("EGL init failed");
                    }
                }
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            g_window_ready = false;
            if (g_vglite_initialized) {
                term_vglite();
                g_vglite_initialized = false;
            }
            if (g_egl_initialized) {
                term_egl();
                g_egl_initialized = false;
            }
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            break;
            
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
            break;
    }
}

// ============================================================================
// Main Entry Point
// ============================================================================
void android_main(android_app* app) {
    LOGI("=== VGLite Blit Demo (Android) ===");
    LOGI("Square size: %dx%d, Interval: %dms", SQUARE_SIZE, SQUARE_SIZE, MOVE_INTERVAL_MS);
    
    app->onAppCmd = handle_app_cmd;
    
    bool running = true;
    int frame_count = 0;
    
    LOGI("Entering main loop...");
    
    while (running) {
        int ident;
        int events;
        android_poll_source* source;
        
        int timeout = (g_window_ready && g_egl_initialized && g_vglite_initialized) ? 0 : -1;
        
        while ((ident = ALooper_pollOnce(timeout, nullptr, &events, (void**)&source)) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
            
            if (app->destroyRequested != 0) {
                running = false;
                break;
            }
            
            timeout = (g_window_ready && g_egl_initialized && g_vglite_initialized) ? 0 : -1;
        }
        
        if (!running) break;
        
        if (g_window_ready && g_egl_initialized && g_vglite_initialized) {
            const char* corner_name = (g_corner_index == 0) ? "Top-Left" :
                                     (g_corner_index == 1) ? "Top-Right" :
                                     (g_corner_index == 2) ? "Bottom-Right" : "Bottom-Left";
            const char* color_name = (g_color_index == 0) ? "Red" :
                                    (g_color_index == 1) ? "Green" :
                                    (g_color_index == 2) ? "Blue" :
                                    (g_color_index == 3) ? "Yellow" : "Cyan";
            
            LOGI("Cycle %d: %s at %s", g_cycle_count + 1, color_name, corner_name);
            render_frame(g_corner_index, g_color_index);
            
            g_corner_index = (g_corner_index + 1) % 4;
            g_color_index = (g_color_index + 1) % NUM_COLORS;
            if (g_corner_index == 0) {
                g_cycle_count++;
            }
            
            usleep(MOVE_INTERVAL_MS * 1000);
        }
    }
    
    LOGI("Cleaning up...");
    if (g_vglite_initialized) {
        term_vglite();
    }
    if (g_egl_initialized) {
        term_egl();
    }
    LOGI("Demo finished");
}
