/**
 * @file linear_grad_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/linearGrad/linearGrad.c
 * 
 * Resolution: 480 x 320
 * Format: VG_LITE_RGB565
 * Related APIs: vg_lite_init_grad/vg_lite_set_grad/vg_lite_update_grad/vg_lite_draw_grad
 * Description: Test linear gradient filling.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 480;
static const int fb_height = 320;

// Path data from original (lines 39-45)
static char path_data[] = {
    2, 0, 0,   // moveto   0,0
    4, 80, 0,  // lineto   80,0
    4, 80, 80, // lineto   80,80
    4, 0, 80,  // lineto   0,80
    0,         // end
};

// Path struct from original (lines 47-56)
static vg_lite_path_t path = {
    { 0,   0, // left,top
      100, 100 }, // right,bottom
    VG_LITE_HIGH, // quality
    VG_LITE_S8, // -128 to 127 coordinate range
    {0}, // uploaded
    sizeof(path_data), // path length
    path_data, // path data
    1
};

// Gradient ramps from original (line 77)
static uint32_t ramps[] = { 0xffeeaa00, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xff00ffaa, 0xff00ffee};
static uint32_t stops[] = { 28, 86, 142, 220, 270, 300};

// Test fixture for LinearGrad tests
class LinearGradTest : public ::testing::Test {
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

// Test: Linear gradient test (main test from original linearGrad.c)
TEST_F(LinearGradTest, LinearGrad) {
    vg_lite_filter_t filter;
    vg_lite_linear_gradient_t grad;
    vg_lite_matrix_t *matGrad;
    vg_lite_matrix_t matPath;
    
    // Initialize vg_lite engine (from original line 85)
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    filter = VG_LITE_FILTER_POINT;
    
    // Allocate the off-screen buffer (from original lines 90-94)
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGB565;
    
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Initialize gradient (from original lines 100-105)
    memset(&grad, 0, sizeof(grad));
    error_ = vg_lite_init_grad(&grad);
    if (error_ != VG_LITE_SUCCESS) {
        GTEST_SKIP() << "Linear gradient is not supported";
    }
    
    // Set gradient (from original line 107)
    error_ = vg_lite_set_grad(&grad, (sizeof(ramps)/sizeof(ramps[0])), ramps, stops);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_set_grad failed";
    
    // Update gradient (from original line 108)
    error_ = vg_lite_update_grad(&grad);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_update_grad failed";
    
    // Get gradient matrix (from original line 109)
    matGrad = vg_lite_get_grad_matrix(&grad);
    ASSERT_NE(matGrad, nullptr) << "vg_lite_get_grad_matrix returned null";
    
    // Identity on gradient matrix (from original line 110)
    error_ = vg_lite_identity(matGrad);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_identity matGrad failed";
    
    // Clear the buffer with white (from original line 113)
    error_ = vg_lite_clear(&buffer_, nullptr, 0xffffffff);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Identity on path matrix (from original line 114)
    error_ = vg_lite_identity(&matPath);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_identity matPath failed";
    
    // Scale (from original line 115)
    error_ = vg_lite_scale(3.0f, 3.0f, &matPath);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
    
    // Rotate gradient matrix (from original line 116)
    error_ = vg_lite_rotate(20.0f, matGrad);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_rotate failed";
    
    // Draw gradient (from original line 119)
    error_ = vg_lite_draw_grad(&buffer_, &path, VG_LITE_FILL_EVEN_ODD, &matPath, &grad, VG_LITE_BLEND_NONE);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_draw_grad failed";
    
    // Finish (from original line 120)
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
    
    // Clear gradient (from original line 121)
    error_ = vg_lite_clear_grad(&grad);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear_grad failed";
}

} // anonymous namespace
