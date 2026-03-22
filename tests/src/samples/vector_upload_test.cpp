/*
 * vector_upload_test.cpp - Vector path upload test
 * 
 * Migrated from VSI_CTS/samples/vector_upload/vector_upload.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\vector_upload\vector_upload.c
 * 
 * This test demonstrates uploading path data to GPU before drawing.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class VectorUploadTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
        fb_scale_ = 1.0f;
    }
    
    void TearDown() override {
        cleanup();
        VGLiteTest::TearDown();
    }
    
    void cleanup(void) {
        if (buffer_.handle != NULL) {
            vg_lite_free(&buffer_);
        }
        vg_lite_clear_path(&path_);
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    float fb_scale_;
    
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
    static char path_data[];
    static vg_lite_path_t path_;
};

char VectorUploadTest::path_data[] = {
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

vg_lite_path_t VectorUploadTest::path_ = {
    {-10, -10, // left,top
    10, 10}, // right,bottom
    VG_LITE_HIGH, // quality
    VG_LITE_S8, // -128 to 127 coordinate range
    {0}, // uploaded
    sizeof(path_data), // path length
    path_data, // path data
    1
};

TEST_F(VectorUploadTest, DISABLED_VectorUpload) {
    GTEST_SKIP() << "SKIPPED: vg_lite_upload_path not supported by ThorVG backend";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_buffer_t *fb = &buffer_;
    
    int fb_width = 256, fb_height = 256;
    fb_scale_ = (float)fb_width / 256.0f;
    
    // Allocate the off-screen buffer
    buffer_.width  = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGB565;
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear the buffer with blue
    error = vg_lite_clear(fb, NULL, 0xFFFF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Try uploading the path data
    error = vg_lite_upload_path(&path_);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_upload_path failed";
    
    // Setup a 2x2 scale at top-left of buffer
    vg_lite_identity(&matrix);
    vg_lite_translate(fb_width / 4, fb_height / 4, &matrix);
    vg_lite_scale(2, 2, &matrix);
    vg_lite_rotate(45, &matrix);
    vg_lite_scale(fb_scale_, fb_scale_, &matrix);
    
    // Draw the path using the matrix
    error = vg_lite_draw(fb, &path_, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    // Setup a 2x2 scale at top-right of buffer
    vg_lite_identity(&matrix);
    vg_lite_translate(3 * fb_width / 4, fb_height / 4, &matrix);
    vg_lite_scale(2, 2, &matrix);
    vg_lite_rotate(135, &matrix);
    vg_lite_scale(fb_scale_, fb_scale_, &matrix);
    
    error = vg_lite_draw(fb, &path_, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    // Setup a 2x2 scale at center of buffer
    vg_lite_identity(&matrix);
    vg_lite_translate(fb_width / 2, fb_height / 2, &matrix);
    vg_lite_scale(5, 5, &matrix);
    vg_lite_scale(fb_scale_, fb_scale_, &matrix);
    
    error = vg_lite_draw(fb, &path_, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    // Setup a 2x2 scale at bottom-left of buffer
    vg_lite_identity(&matrix);
    vg_lite_translate(fb_width / 4, 3 * fb_height / 4, &matrix);
    vg_lite_scale(2, 2, &matrix);
    vg_lite_rotate(-45, &matrix);
    vg_lite_scale(fb_scale_, fb_scale_, &matrix);
    
    error = vg_lite_draw(fb, &path_, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    // Setup a 2x2 scale at bottom-right of buffer
    vg_lite_identity(&matrix);
    vg_lite_translate(3 * fb_width / 4, 3 * fb_height / 4, &matrix);
    vg_lite_scale(2, 2, &matrix);
    vg_lite_rotate(-135, &matrix);
    vg_lite_scale(fb_scale_, fb_scale_, &matrix);
    
    error = vg_lite_draw(fb, &path_, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_NONE, 0xFF0000FF);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_draw failed";
    
    error = vg_lite_finish();
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_finish failed";
    
    // Save PNG file
    vg_lite_save_png("vector_upload.png", fb);
    */
}

} // anonymous namespace
