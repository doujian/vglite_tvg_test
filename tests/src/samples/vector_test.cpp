/**
 * @file vector_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/vector/vector.c
 * 
 * Resolution: 256 x 256
 * Format: VG_LITE_RGB565
 * Transformation: Translate/Scale
 * Alpha Blending: None
 * Related APIs: vg_lite_clear/vg_lite_translate/vg_lite_scale/vg_lite_draw
 * Description: Draw a polygon vector graphic with high render quality on blue buffer.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 256;
static const int fb_height = 256;
static const float DEFAULT_SIZE = 256.0f;

// Test fixture for Vector tests
class VectorTest : public ::testing::Test {
protected:
    vg_lite_buffer_t buffer_;
    vg_lite_error_t error_;
    
    void SetUp() override {
        memset(&buffer_, 0, sizeof(buffer_));
        error_ = VG_LITE_SUCCESS;
    }
    
    void TearDown() override {
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        vg_lite_close();
    }
};

// Path data from original (star shape)
/*
            *-----*
           /       \
          /         \
         *           *
         |          /
         |         X
         |          \
         *           *
          \         /
           \       /
            *-----*
*/
static char path_data[] = {
    2, -5, -10, // moveto   -5,-10
    4, 5, -10,  // lineto    5,-10
    4, 10, -5,  // lineto   10, -5
    4, 0, 0,    // lineto    0,  0
    4, 10, 5,   // lineto   10,  5
    4, 5, 10,   // lineto    5, 10
    4, -5, 10,  // lineto   -5, 10
    4, -10, 5,  // lineto  -10,  5
    4, -10, -5, // lineto  -10, -5
    0, // end
};

static vg_lite_path_t path = {
    {-10, -10, // left,top
     10, 10},  // right,bottom
    VG_LITE_HIGH,    // quality
    VG_LITE_S8,      // -128 to 127 coordinate range
    {0},             // uploaded
    sizeof(path_data), // path length
    path_data,       // path data
    1
};

// Test: Vector_Draw - Draw polygon vector graphic (from original main function)
TEST_F(VectorTest, Vector_Draw) {
    vg_lite_matrix_t matrix;
    float fb_scale = (float)fb_width / DEFAULT_SIZE;
    
    /* Initialize the draw. */
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    /* Allocate the off-screen buffer. */
    buffer_.width  = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGB565;
    
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear the buffer with blue.
    error_ = vg_lite_clear(&buffer_, NULL, 0xFFFF0000);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // *** DRAW ***
    // Setup a 10x10 scale at center of buffer.
    vg_lite_identity(&matrix);
    // Translate the matrix to the center of the buffer.
    error_ = vg_lite_translate(fb_width / 2.0f, fb_height / 2.0f, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_translate failed";
    
    // Scale up matrix with 10 times.
    error_ = vg_lite_scale(10, 10, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
    
    // Scale matrix according to buffer size.
    error_ = vg_lite_scale(fb_scale, fb_scale, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
    
    // Draw the path using the matrix.
    error_ = vg_lite_draw(&buffer_, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    if (error_ == VG_LITE_NOT_SUPPORT) {
        GTEST_SKIP() << "SKIPPED: vg_lite_draw with VG_LITE_S8 path format not supported";
    }
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
    
    // Save PNG file (from original line 126)
    vg_lite_save_png("vector.png", &buffer_);
    
    // Cleanup path
    vg_lite_clear_path(&path);
}

} // anonymous namespace
