/**
 * @file rotate_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/rotate/rotate.c
 * 
 * Resolution: 320 x 480
 * Format: VG_LITE_RGB565
 * Transformation: Rotate/Scale/Translate
 * Alpha Blending: None
 * Related APIs: vg_lite_clear/vg_lite_translate/vg_lite_scale/vg_lite_rotate/vg_lite_blit
 * Description: Load outside landscape image, then blit it to blue dest buffer with rotate/scale/translate.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 320;
static const int fb_height = 480;

// Test fixture for Rotate tests
class RotateTest : public ::testing::Test {
protected:
    vg_lite_buffer_t buffer_;
    vg_lite_buffer_t image_;
    vg_lite_error_t error_;
    
    void SetUp() override {
        memset(&buffer_, 0, sizeof(buffer_));
        memset(&image_, 0, sizeof(image_));
        error_ = VG_LITE_SUCCESS;
    }
    
    void TearDown() override {
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        if (image_.handle != nullptr) {
            vg_lite_free(&image_);
        }
        vg_lite_close();
    }
};

// Test: Rotate test (main test from original rotate.c)
TEST_F(RotateTest, Rotate) {
    vg_lite_filter_t filter;
    vg_lite_matrix_t matrix;
    
    // Initialize vg_lite engine (from original line 68)
    error_ = vg_lite_init(32, 32);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Set image filter type (from original line 71)
    filter = VG_LITE_FILTER_POINT;
    
    // Load the image (from original lines 74-78)
    // Resource file located in tests/resources/rotate/
    if (vg_lite_load_raw(&image_, "tests/resources/rotate/landscape.raw") != 0) {
        GTEST_SKIP() << "SKIPPED: resources/rotate/landscape.raw file not found";
    }
    
    // Allocate the off-screen buffer (from original lines 84-89)
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGB565;
    
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear the buffer with blue (from original line 92)
    error_ = vg_lite_clear(&buffer_, nullptr, 0xFFFF0000);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Build a 33 degree rotation matrix from the center of the buffer (from original lines 94-103)
    vg_lite_identity(&matrix);
    
    // Translate the matrix to the center of the buffer (from original line 96)
    error_ = vg_lite_translate(fb_width / 2.0f, fb_height / 2.0f, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_translate failed";
    
    // Do a 33 degree rotation (from original line 98)
    error_ = vg_lite_rotate(33.0f, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_rotate failed";
    
    // Translate the matrix again to adjust matrix location (from original line 100)
    error_ = vg_lite_translate(fb_width / -2.0f, fb_height / -2.0f, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_translate failed";
    
    // Scale matrix according to buffer size (from original lines 102-103)
    error_ = vg_lite_scale((vg_lite_float_t)fb_width / (vg_lite_float_t)image_.width,
                           (vg_lite_float_t)fb_height / (vg_lite_float_t)image_.height, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
    
    // Blit the image using the matrix (from original line 106)
    error_ = vg_lite_blit(&buffer_, &image_, &matrix, VG_LITE_BLEND_NONE, 0, filter);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_blit failed";
    
    // Finish (from original line 107)
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
}

} // anonymous namespace
