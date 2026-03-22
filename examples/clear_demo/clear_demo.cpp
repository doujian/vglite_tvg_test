/**
 * @file clear_demo.cpp
 * @brief vg_lite_clear demo with GLFW window
 * 
 * This demo demonstrates the vg_lite_clear function by cycling through
 * five colors (red, green, blue, yellow, cyan) in a GLFW window.
 * 
 * Press ESC or close the window to exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// GLFW for window management
#include <GLFW/glfw3.h>

// OpenGL constants that may not be defined on some systems
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

// C++ standard library
#include <chrono>
#include <thread>

// ============================================================================
// Configuration
// ============================================================================
static const int WINDOW_WIDTH = 640;
static const int WINDOW_HEIGHT = 480;
static const int BUFFER_WIDTH = 256;
static const int BUFFER_HEIGHT = 256;
static const int COLOR_SWITCH_INTERVAL_MS = 1000;

// Colors in ARGB format (0xAARRGGBB)
// Note: vg_lite uses ARGB internally, but OpenGL uses GL_BGRA for texture upload.
// To get correct colors displayed, we use ABGR format here which matches GL_BGRA.
static const vg_lite_color_t COLORS[] = {
    0xFF0000FF,  // Red    (ABGR: A=FF, B=00, G=00, R=FF)
    0xFF00FF00,  // Green  (ABGR: A=FF, B=00, G=FF, R=00)
    0xFFFF0000,  // Blue   (ABGR: A=FF, B=FF, G=00, R=00)
    0xFF00FFFF,  // Yellow (ABGR: A=FF, B=00, G=FF, R=FF)
    0xFFFFFF00   // Cyan   (ABGR: A=FF, B=FF, G=FF, R=00)
};
static const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

// ============================================================================
// Global state
// ============================================================================
static vg_lite_buffer_t g_buffer = {0};
static GLuint g_texture = 0;

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
// OpenGL texture setup
// ============================================================================
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
// Update texture from vg_lite buffer
// ============================================================================
static void update_texture() {
    glBindTexture(GL_TEXTURE_2D, g_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
                    GL_BGRA, GL_UNSIGNED_BYTE, g_buffer.memory);
}

// ============================================================================
// Render texture to window
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
    printf("=== vg_lite_clear Demo ===\n");
    printf("Window: %dx%d, Buffer: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT);
    printf("Colors: %d, Interval: %dms\n", NUM_COLORS, COLOR_SWITCH_INTERVAL_MS);
    printf("Press ESC or close window to exit.\n\n");
    
    // ------------------------------------------------------------------------
    // Initialize vg_lite
    // ------------------------------------------------------------------------
    printf("Initializing vg_lite...\n");
    vg_lite_error_t error = vg_lite_init(BUFFER_WIDTH, BUFFER_HEIGHT);
    check_vg_lite_error(error, "vg_lite_init");
    
    // ------------------------------------------------------------------------
    // Allocate buffer
    // ------------------------------------------------------------------------
    printf("Allocating buffer...\n");
    memset(&g_buffer, 0, sizeof(g_buffer));
    g_buffer.width = BUFFER_WIDTH;
    g_buffer.height = BUFFER_HEIGHT;
    g_buffer.format = VG_LITE_BGRA8888;
    
    error = vg_lite_allocate(&g_buffer);
    check_vg_lite_error(error, "vg_lite_allocate");
    
    printf("Buffer allocated: %dx%d, stride=%d, format=BGRA8888\n", 
           g_buffer.width, g_buffer.height, g_buffer.stride);
    
    // ------------------------------------------------------------------------
    // Initialize GLFW
    // ------------------------------------------------------------------------
    printf("Initializing GLFW...\n");
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        fprintf(stderr, "Error: Failed to initialize GLFW\n");
        vg_lite_free(&g_buffer);
        vg_lite_close();
        return 1;
    }
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          "vg_lite_clear Demo", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Error: Failed to create GLFW window\n");
        glfwTerminate();
        vg_lite_free(&g_buffer);
        vg_lite_close();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    
    // Setup OpenGL
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    setup_texture();
    
    printf("GLFW window created successfully.\n\n");
    
    // ------------------------------------------------------------------------
    // Main loop - color cycling
    // ------------------------------------------------------------------------
    int color_index = 0;
    int cycle_count = 0;
    
    while (!glfwWindowShouldClose(window)) {
        // Get current color
        vg_lite_color_t color = COLORS[color_index];
        const char* color_name = (color_index == 0) ? "Red" :
                                 (color_index == 1) ? "Green" :
                                 (color_index == 2) ? "Blue" :
                                 (color_index == 3) ? "Yellow" : "Cyan";
        
        printf("Cycle %d: %s (0x%08X)\n", cycle_count + 1, color_name, color);
        
        // Clear buffer with current color
        error = vg_lite_clear(&g_buffer, nullptr, color);
        check_vg_lite_error(error, "vg_lite_clear");
        
        // Finish rendering
        error = vg_lite_finish();
        check_vg_lite_error(error, "vg_lite_finish");
        
        // Update OpenGL texture
        update_texture();
        
        // Render to window
        render_texture();
        glfwSwapBuffers(window);
        
        // Poll events while waiting
        auto start = std::chrono::steady_clock::now();
        while (true) {
            glfwPollEvents();
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start);
            
            if (elapsed.count() >= COLOR_SWITCH_INTERVAL_MS) {
                break;
            }
            
            // Check if window should close
            if (glfwWindowShouldClose(window)) {
                break;
            }
            
            // Small sleep to avoid busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Move to next color
        color_index = (color_index + 1) % NUM_COLORS;
        if (color_index == 0) {
            cycle_count++;
        }
    }
    
    // ------------------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------------------
    printf("\nCleaning up...\n");
    
    // Cleanup OpenGL
    if (g_texture) {
        glDeleteTextures(1, &g_texture);
    }
    
    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    
    // Cleanup vg_lite
    error = vg_lite_free(&g_buffer);
    check_vg_lite_error(error, "vg_lite_free");
    
    error = vg_lite_close();
    check_vg_lite_error(error, "vg_lite_close");
    
    printf("Demo finished successfully.\n");
    return 0;
}
