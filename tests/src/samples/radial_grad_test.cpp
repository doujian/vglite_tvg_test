/**
 * @file radial_grad_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/radialGrad/radialGrad.c
 * 
 * Resolution: 480 x 640
 * Format: VG_LITE_ARGB8888
 * Related APIs: vg_lite_set_radial_grad/vg_lite_update_radial_grad/vg_lite_draw_radial_grad
 * Description: Test radial gradient filling with different spread modes.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 480;
static const int fb_height = 640;

// Spread modes from original (lines 38-43)
static vg_lite_gradient_spreadmode_t spreadmode[] = {
    VG_LITE_GRADIENT_SPREAD_FILL,
    VG_LITE_GRADIENT_SPREAD_PAD,
    VG_LITE_GRADIENT_SPREAD_REPEAT,
    VG_LITE_GRADIENT_SPREAD_REFLECT,
};
static const int kSpreadModeCount = 4;

// Path data from original (lines 45-51)
static char path_data[] = {
    2, 0, 0,
    4, 120, 0,
    4, 120, 120,
    4, 0, 120,
    0,
};

// Path struct from original (lines 53-62)
static vg_lite_path_t path = {
    { 0,   0,
      120, 120 },
    VG_LITE_HIGH,
    VG_LITE_S8,
    {0},
    sizeof(path_data),
    path_data,
    1
};

// Radial gradient parameter from original (line 80)
static vg_lite_radial_gradient_parameter_t radialGradient = { 120.0f, 120.0f, 115.0f, 120.0f, 120.0f };

// Color ramp from original (lines 82-104)
static vg_lite_color_ramp_t vgColorRamp[] = {
    { 0.0f, 0.95f, 0.2f, 0.0f, 0.9f },
    { 0.2f, 0.85f, 0.3f, 0.2f, 1.0f },
    { 0.4f, 0.75f, 0.1f, 0.3f, 1.0f },
    { 0.55f, 0.9f, 0.2f, 0.2f, 1.0f },
    { 0.95f, 0.9f, 0.9f, 0.9f, 1.0f },
};
static const int kColorRampCount = sizeof(vgColorRamp) / sizeof(vgColorRamp[0]);

// Test fixture for RadialGrad tests
class RadialGradTest : public ::testing::Test {
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

// Test: Radial gradient test (main test from original radialGrad.c)
TEST_F(RadialGradTest, RadialGrad) {
    vg_lite_radial_gradient_t grad;
    vg_lite_matrix_t* matGrad;
    vg_lite_matrix_t matPath;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    
    // Initialize vg_lite engine (from original line 110)
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Check radial gradient feature (from original lines 111-114)
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RADIAL_GRADIENT)) {
        GTEST_SKIP() << "radialGradient is not supported";
    }
    
    // Allocate buffer (from original lines 117-121)
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_ARGB8888;
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Test each spread mode (from original lines 124-145)
    for (int fcount = 0; fcount < kSpreadModeCount; fcount++) {
        memset(&grad, 0, sizeof(grad));
        
        // Set radial gradient (from original line 129)
        error_ = vg_lite_set_radial_grad(&grad, kColorRampCount, vgColorRamp, 
                                          radialGradient, spreadmode[fcount], 1);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_set_radial_grad failed at fcount=" << fcount;
        
        // Update radial gradient (from original line 130)
        error_ = vg_lite_update_radial_grad(&grad);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_update_radial_grad failed at fcount=" << fcount;
        
        // Get gradient matrix (from original line 132)
        matGrad = vg_lite_get_radial_grad_matrix(&grad);
        ASSERT_NE(matGrad, nullptr) << "vg_lite_get_radial_grad_matrix returned null";
        
        // Identity on gradient matrix (from original line 133)
        error_ = vg_lite_identity(matGrad);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_identity matGrad failed";
        
        // Clear buffer (from original line 134)
        error_ = vg_lite_clear(&buffer_, nullptr, 0xffffffff);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
        
        // Identity on path matrix (from original line 135)
        error_ = vg_lite_identity(&matPath);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_identity matPath failed";
        
        // Scale (from original line 136)
        error_ = vg_lite_scale(2.0f, 2.0f, &matPath);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_scale failed";
        
        // Draw radial gradient (from original line 138)
        error_ = vg_lite_draw_radial_grad(&buffer_, &path, VG_LITE_FILL_EVEN_ODD, 
                                           &matPath, &grad, 0, VG_LITE_BLEND_NONE, 
                                           VG_LITE_FILTER_LINEAR);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_draw_radial_grad failed at fcount=" << fcount;
        
        // Finish (from original line 139)
        error_ = vg_lite_finish();
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
        
        // Clear gradient (from original line 140)
        error_ = vg_lite_clear_radial_grad(&grad);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear_radial_grad failed";
    }
}

} // anonymous namespace
