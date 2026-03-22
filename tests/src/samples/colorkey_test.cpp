/**
 * @file colorkey_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/colorkey/colorkey.c
 * 
 * Resolution: 320 x 480
 * Format: VG_LITE_RGBA8888
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

// Color key from original (lines 59-65)
static vg_lite_color_key4_t colorkey = {
    {1, 0, 0, 0, 0x55, 100, 100, 100},
    {1, 0, 0, 0, 0x55, 100, 100, 100},
    {1, 0, 0, 0, 0x55, 0, 0, 0},
    {1, 0, 0, 0, 0x55, 0, 0, 0},
};

// Test fixture for ColorKey tests
class ColorKeyTest : public ::testing::Test {
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

// Test: Color key test (main test from original colorkey.c)
TEST_F(ColorKeyTest, ColorKey) {
    vg_lite_filter_t filter;
    vg_lite_matrix_t matrix;
    
    // Initialize vg_lite (from original line 73)
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Check color key feature (from original lines 75-78)
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_COLOR_KEY)) {
        GTEST_SKIP() << "color key is not supported";
    }
    
    filter = VG_LITE_FILTER_POINT;
    
    // Load image (from original lines 82-86)
    // Resource file located in tests/resources/colorkey/
    if (vg_lite_load_raw(&image_, "resources/colorkey/landscape.raw") != 0) {
        GTEST_SKIP() << "SKIPPED: resources/colorkey/landscape.raw file not found";
    }
    
    // Allocate buffer (from original lines 90-95)
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGBA8888;
    
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Set color key (from original line 96)
    error_ = vg_lite_set_color_key(colorkey);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_set_color_key failed";
    
    // Clear buffer (from original line 98)
    error_ = vg_lite_clear(&buffer_, nullptr, 0xFF323232);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Setup matrix (from original lines 99-101)
    vg_lite_identity(&matrix);
    error_ = vg_lite_scale((vg_lite_float_t)fb_width / (vg_lite_float_t)image_.width,
                           (vg_lite_float_t)fb_height / (vg_lite_float_t)image_.height, &matrix);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
    
    // Blit (from original line 103)
    error_ = vg_lite_blit(&buffer_, &image_, &matrix, VG_LITE_BLEND_NONE, 0, filter);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_blit failed";
    
    // Finish (from original line 104)
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
}

} // anonymous namespace
