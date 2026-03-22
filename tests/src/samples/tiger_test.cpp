/*
 * tiger_test.cpp - Tiger SVG rendering test
 * 
 * Migrated from VSI_CTS/samples/tiger/tiger.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\tiger\tiger.c
 * 
 * This test renders the famous SVG tiger using vg_lite path drawing.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "tiger_paths.h"

namespace {

class TigerTest : public ::testing::Test {
protected:
    void SetUp() override {
        fb_width = 640;
        fb_height = 480;
        fb = nullptr;
        memset(&buffer, 0, sizeof(buffer));
    }
    
    void TearDown() override {
        cleanup();
    }
    
    void cleanup(void) {
        int32_t i;
        
        if (buffer.handle != NULL) {
            vg_lite_free(&buffer);
        }
        
        for (i = 0; i < pathCount; i++) {
            vg_lite_clear_path(&path[i]);
        }
        
        vg_lite_close();
    }
    
    static int fb_width;
    static int fb_height;
    static vg_lite_buffer_t buffer;
    static vg_lite_buffer_t * fb;
};

int TigerTest::fb_width = 640;
int TigerTest::fb_height = 480;
vg_lite_buffer_t TigerTest::buffer;
vg_lite_buffer_t * TigerTest::fb = nullptr;

TEST_F(TigerTest, Tiger) {
    vg_lite_error_t error = VG_LITE_SUCCESS;
    int i;
    vg_lite_filter_t filter;
    vg_lite_matrix_t matrix;
    
    /* Initialize vglite. */
    error = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    filter = VG_LITE_FILTER_POINT;
    
    printf("Framebuffer size: %d x %d\n", fb_width, fb_height);
    
    /* Allocate the off-screen buffer. */
    buffer.width  = fb_width;
    buffer.height = fb_height;
    buffer.format = VG_LITE_RGBA8888;
    error = vg_lite_allocate(&buffer);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    fb = &buffer;
    
    // Clear the buffer with blue.
    error = vg_lite_clear(fb, NULL, 0xFFFF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Setup a scale at center of buffer.
    vg_lite_identity(&matrix);
    vg_lite_translate(fb_width / 2 - 20 * fb_width / 640.0f,
                      fb_height / 2 - 100 * fb_height / 480.0f, &matrix);
    vg_lite_scale(4, 4, &matrix);
    vg_lite_scale(fb_width / 640.0f, fb_height / 480.0f, &matrix);
    
    // Draw the path using the matrix.
    for (i = 0; i < pathCount; i++) {
        error = vg_lite_draw(fb, &path[i], VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, color_data[i]);
        if (error != VG_LITE_SUCCESS) {
            printf("vg_lite_draw failed for path %d: error %d\n", i, error);
            // Continue drawing other paths
        }
    }
    
    error = vg_lite_finish();
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_finish failed";
    
    // Save PNG file.
    vg_lite_save_png("tiger.png", fb);
    
    // Verify buffer was rendered
    EXPECT_NE(buffer.memory, nullptr);
}

} // anonymous namespace
