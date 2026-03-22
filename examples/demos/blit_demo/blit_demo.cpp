/**
 * @file blit_demo.cpp
 * @brief vg_lite_blit demo with GLFW window
 * 
 * This demo demonstrates the vg_lite_blit function by displaying
 * a 100x100 colored square that moves clockwise around the four corners
 * of the window every 1 second.
 * 
 * Press ESC or close the window to exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLFW for window management
#include <GLFW/glfw3.h>

// GLFW native headers for platform-specific GL context
#if defined(_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
    #include <GLFW/glfw3native.h>
    #include <wingdi.h>
#endif

// OpenGL constants
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

// vg_lite API
extern "C" {
#include "vg_lite.h"
}

// GL backend configuration
#include "vglite_gl_config.h"

// C++ standard library
#include <chrono>
#include <thread>

// ============================================================================
// Configuration
// ============================================================================
static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;

#if VGLITE_USE_GL_BACKEND
// GL backend: render directly to window framebuffer
static const int BUFFER_WIDTH = WINDOW_WIDTH;
static const int BUFFER_HEIGHT = WINDOW_HEIGHT;
#else
// SW backend: render to offscreen buffer then upload to texture
static const int BUFFER_WIDTH = 256;
static const int BUFFER_HEIGHT = 256;
#endif

static const int MOVE_INTERVAL_MS = 1000;  // 1 second per corner

// Square size
static const int SQUARE_SIZE = 100;

// Colors in ABGR format (0xAARRGGBB)
static const vg_lite_color_t COLORS[] = {
    0xFF0000FF,  // Red    (ABGR)
    0xFF00FF00,  // Green  (ABGR)
    0xFFFF0000,  // Blue   (ABGR)
    0xFF00FFFF,  // Yellow (ABGR)
    0xFFFFFF00   // Cyan   (ABGR)
};
static const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

// Four corners (clockwise: top-left -> top-right -> bottom-right -> bottom-left)
struct Corner {
    float x;
    float y;
};

// ============================================================================
// Global state
// ============================================================================
#if !VGLITE_USE_GL_BACKEND
// SW backend only: buffer and texture for CPU rendering
static vg_lite_buffer_t g_buffer = {0};
static GLuint g_texture = 0;
#endif

// Source buffer containing the colored square
static vg_lite_buffer_t g_source_buffer = {0};

// ============================================================================
// Error handling
// ============================================================================
static void check_vg_lite_error(vg_lite_error_t error, const char* operation) {
    if (error != VG_LITE_SUCCESS) {
        fprintf(stderr, "Error: %s failed with error %d\n", operation, error);
        exit(1);
    }
}

// ============================================================================
// Create source buffer with colored square
// ============================================================================
static void create_source_buffer() {
    printf("Creating source buffer (%dx%d colored square)...\n", SQUARE_SIZE, SQUARE_SIZE);
    
    memset(&g_source_buffer, 0, sizeof(g_source_buffer));
    g_source_buffer.width = SQUARE_SIZE;
    g_source_buffer.height = SQUARE_SIZE;
    g_source_buffer.format = VG_LITE_BGRA8888;
    
    vg_lite_error_t error = vg_lite_allocate(&g_source_buffer);
    check_vg_lite_error(error, "vg_lite_allocate (source)");
    
    // Fill with the first color (will be updated during animation)
    vg_lite_color_t color = COLORS[0];
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
    
    printf("Source buffer created with color 0x%08X\n", color);
}

// ============================================================================
// Update source buffer color
// ============================================================================
static void update_source_color(int color_index) {
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
// OpenGL texture setup (SW backend only)
// ============================================================================
#if !VGLITE_USE_GL_BACKEND
static void setup_texture() {
    glGenTextures(1, &g_texture);
    glBindTexture(GL_TEXTURE_2D, g_texture);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Allocate texture storage
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BUFFER_WIDTH, BUFFER_HEIGHT, 
                 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
}

// ============================================================================
// Update texture from vg_lite buffer (SW backend only)
// ============================================================================
static void update_texture() {
    glBindTexture(GL_TEXTURE_2D, g_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
                    GL_BGRA, GL_UNSIGNED_BYTE, g_buffer.memory);
}

// ============================================================================
// Render texture to window (SW backend only)
// ============================================================================
static void render_texture() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Setup orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texture);
    
    // Calculate centered position
    int x = (WINDOW_WIDTH - BUFFER_WIDTH) / 2;
    int y = (WINDOW_HEIGHT - BUFFER_HEIGHT) / 2;
    
    // Draw textured quad
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + BUFFER_WIDTH, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + BUFFER_WIDTH, y + BUFFER_HEIGHT);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + BUFFER_HEIGHT);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}
#endif

// ============================================================================
// GLFW error callback
// ============================================================================
static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}

// ============================================================================
// GLFW key callback
// ============================================================================
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// ============================================================================
// Main entry point
// ============================================================================
int main(int argc, char** argv) {
    printf("=== vg_lite_blit Demo ===\n");
#if VGLITE_USE_GL_BACKEND
    printf("Backend: OpenGL (GPU direct rendering)\n");
#else
    printf("Backend: Software (CPU rendering with texture upload)\n");
#endif
    printf("Window: %dx%d, Square: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT, SQUARE_SIZE, SQUARE_SIZE);
    printf("Move interval: %dms\n", MOVE_INTERVAL_MS);
    printf("Press ESC or close window to exit.\n\n");
    
    // ------------------------------------------------------------------------
    // Initialize GLFW
    // ------------------------------------------------------------------------
    printf("Initializing GLFW...\n");
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        fprintf(stderr, "Error: Failed to initialize GLFW\n");
        return 1;
    }
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          "vg_lite_blit Demo", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Error: Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    
    printf("GLFW window created successfully.\n");
    
    // ------------------------------------------------------------------------
    // Initialize vg_lite
    // ------------------------------------------------------------------------
    printf("Initializing vg_lite...\n");
    vg_lite_error_t error = vg_lite_init(BUFFER_WIDTH, BUFFER_HEIGHT);
    check_vg_lite_error(error, "vg_lite_init");
    
#if VGLITE_USE_GL_BACKEND
    // GL backend: Set external GL context
    printf("Setting up GL backend context...\n");
    
#if defined(_WIN32)
    HGLRC gl_context = wglGetCurrentContext();
    HDC gl_dc = wglGetCurrentDC();
    printf("WGL Context: %p, DC: %p\n", gl_context, gl_dc);
#endif
    error = vg_lite_set_gl_context(nullptr, gl_dc, gl_context, 0);
    check_vg_lite_error(error, "vg_lite_set_gl_context");
#else
    // SW backend: Allocate buffer
    printf("Allocating SW buffer...\n");
    memset(&g_buffer, 0, sizeof(g_buffer));
    g_buffer.width = BUFFER_WIDTH;
    g_buffer.height = BUFFER_HEIGHT;
    g_buffer.format = VG_LITE_BGRA8888;
    
    error = vg_lite_allocate(&g_buffer);
    check_vg_lite_error(error, "vg_lite_allocate");
    
    printf("Buffer allocated: %dx%d, stride=%d, format=BGRA8888\n", 
           g_buffer.width, g_buffer.height, g_buffer.stride);
#endif
    
    // Create source buffer with colored square
    create_source_buffer();
    
    // Setup OpenGL
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
#if !VGLITE_USE_GL_BACKEND
    setup_texture();
#endif
    
    printf("Initialization complete.\n\n");
    
    // ------------------------------------------------------------------------
    // Main loop - square moving around corners
    // ------------------------------------------------------------------------
    // Corner positions (with padding to keep square fully visible)
    const int padding = 50;
    Corner corners[4] = {
        {padding, padding},                                    // Top-left
        {WINDOW_WIDTH - SQUARE_SIZE - padding, padding},       // Top-right
        {WINDOW_WIDTH - SQUARE_SIZE - padding, WINDOW_HEIGHT - SQUARE_SIZE - padding}, // Bottom-right
        {padding, WINDOW_HEIGHT - SQUARE_SIZE - padding}       // Bottom-left
    };
    
    int corner_index = 0;
    int color_index = 0;
    int cycle_count = 0;
    
#if VGLITE_USE_GL_BACKEND
    // GL backend: Dummy buffer for canvas target
    vg_lite_buffer_t fb_buffer = {0};
    fb_buffer.width = WINDOW_WIDTH;
    fb_buffer.height = WINDOW_HEIGHT;
    fb_buffer.format = VG_LITE_BGRA8888;
#endif
    
    while (!glfwWindowShouldClose(window)) {
        // Get current corner position
        Corner corner = corners[corner_index];
        const char* corner_name = (corner_index == 0) ? "Top-Left" :
                                  (corner_index == 1) ? "Top-Right" :
                                  (corner_index == 2) ? "Bottom-Right" : "Bottom-Left";
        const char* color_name = (color_index == 0) ? "Red" :
                                 (color_index == 1) ? "Green" :
                                 (color_index == 2) ? "Blue" :
                                 (color_index == 3) ? "Yellow" : "Cyan";
        
        printf("Cycle %d: %s at %s (%.0f, %.0f)\n", 
               cycle_count + 1, color_name, corner_name, corner.x, corner.y);
        
        // Update source buffer color
        update_source_color(color_index);
        
#if VGLITE_USE_GL_BACKEND
        // GL backend: Clear and blit
        error = vg_lite_clear(&fb_buffer, nullptr, 0xFF000000);  // Clear to black
        check_vg_lite_error(error, "vg_lite_clear");
        
        // Setup matrix for positioning
        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);
        vg_lite_translate(corner.x, corner.y, &matrix);
        
        // Blit the colored square
        error = vg_lite_blit(&fb_buffer, &g_source_buffer, &matrix,
                            VG_LITE_BLEND_SRC_OVER, 0xFFFFFFFF, VG_LITE_FILTER_POINT);
        check_vg_lite_error(error, "vg_lite_blit");
        
        // Finish rendering
        error = vg_lite_finish();
        check_vg_lite_error(error, "vg_lite_finish");
#else
        // SW backend: Clear and blit to buffer
        error = vg_lite_clear(&g_buffer, nullptr, 0xFF000000);  // Clear to black
        check_vg_lite_error(error, "vg_lite_clear");
        
        // Setup matrix for positioning
        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);
        // Scale to fit buffer coordinates
        float scale_x = (float)BUFFER_WIDTH / WINDOW_WIDTH;
        float scale_y = (float)BUFFER_HEIGHT / WINDOW_HEIGHT;
        vg_lite_scale(scale_x, scale_y, &matrix);
        vg_lite_translate(corner.x * scale_x, corner.y * scale_y, &matrix);
        
        // Blit the colored square
        error = vg_lite_blit(&g_buffer, &g_source_buffer, &matrix,
                            VG_LITE_BLEND_SRC_OVER, 0xFFFFFFFF, VG_LITE_FILTER_POINT);
        check_vg_lite_error(error, "vg_lite_blit");
        
        // Finish rendering
        error = vg_lite_finish();
        check_vg_lite_error(error, "vg_lite_finish");
        
        // Update OpenGL texture
        update_texture();
        
        // Render to window
        render_texture();
#endif
        
        glfwSwapBuffers(window);
        
        // Poll events while waiting
        auto start = std::chrono::steady_clock::now();
        while (true) {
            glfwPollEvents();
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
            
            if (elapsed.count() >= MOVE_INTERVAL_MS) {
                break;
            }
            
            if (glfwWindowShouldClose(window)) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Move to next corner and color
        corner_index = (corner_index + 1) % 4;
        color_index = (color_index + 1) % NUM_COLORS;
        if (corner_index == 0) {
            cycle_count++;
        }
    }
    
    // ------------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------------
    printf("\nCleaning up...\n");
    
    // Free source buffer
    error = vg_lite_free(&g_source_buffer);
    check_vg_lite_error(error, "vg_lite_free (source)");
    
#if !VGLITE_USE_GL_BACKEND
    // Cleanup OpenGL texture (SW backend only)
    if (g_texture) {
        glDeleteTextures(1, &g_texture);
    }
    
    // Free buffer (SW backend only)
    error = vg_lite_free(&g_buffer);
    check_vg_lite_error(error, "vg_lite_free");
#endif
    
    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    
    // Cleanup vg_lite
    error = vg_lite_close();
    check_vg_lite_error(error, "vg_lite_close");
    
    printf("Demo finished successfully.\n");
    return 0;
}
