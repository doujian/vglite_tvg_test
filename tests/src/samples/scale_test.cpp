/**
 * @file scale_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/scale/scale.c
 * 
 * Resolution: 500 x 500
 * Format: VG_LITE_RGBA8888
 * Transformation: Scale
 * Alpha Blending: SRC_OVER
 * Related APIs: vg_lite_clear/vg_lite_translate/vg_lite_scale/vg_lite_blit
 * Description: Test scaling operations with different scale factors.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 500;
static const int fb_height = 500;

// Scale factors from original (lines 101-108)
static const float kScaleFactors[] = {
    1.2f, 1.0f, 0.25f, 0.4f, 0.55f, 0.7f, 0.85f, 0.95f
};
static const int kScaleFactorCount = 8;

// Test fixture for Scale tests
class ScaleTest : public ::testing::Test {
protected:
    vg_lite_buffer_t buffer_;
    vg_lite_buffer_t source_;
    vg_lite_error_t error_;
    
    void SetUp() override {
        memset(&buffer_, 0, sizeof(buffer_));
        memset(&source_, 0, sizeof(source_));
        error_ = VG_LITE_SUCCESS;
    }
    
    void TearDown() override {
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        if (source_.handle != nullptr) {
            vg_lite_free(&source_);
        }
        vg_lite_close();
    }
    
    // Helper: Blit operation (from original blitOperation function)
    vg_lite_error_t BlitOperation(vg_lite_buffer_t* src, vg_lite_buffer_t* dst, float scale) {
        vg_lite_filter_t filter = VG_LITE_FILTER_BI_LINEAR;
        vg_lite_matrix_t matrix;
        float sw, sh;
        float tx, ty;
        
        // Clear destination (from original line 63)
        error_ = vg_lite_clear(dst, nullptr, 0xff000000);
        if (error_ != VG_LITE_SUCCESS) return error_;
        
        // Calculate scale and translate (from original lines 65-68)
        sw = (float)src->width * scale;
        sh = (float)src->height * scale;
        tx = ((float)dst->width - sw) / 2.0f;
        ty = ((float)dst->height - sh) / 2.0f;
        
        // Build matrix (from original lines 70-72)
        vg_lite_identity(&matrix);
        error_ = vg_lite_translate(tx, ty, &matrix);
        if (error_ != VG_LITE_SUCCESS) return error_;
        
        error_ = vg_lite_scale(scale, scale, &matrix);
        if (error_ != VG_LITE_SUCCESS) return error_;
        
        // Blit (from original line 74)
        error_ = vg_lite_blit(dst, src, &matrix, VG_LITE_BLEND_SRC_OVER, 0, filter);
        if (error_ != VG_LITE_SUCCESS) return error_;
        
        // Finish (from original line 75)
        error_ = vg_lite_finish();
        return error_;
    }
};

// Test: Scale test (main test from original scale.c)
TEST_F(ScaleTest, Scale) {
    // Initialize vg_lite engine (from original line 87)
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Allocate the off-screen buffer (from original lines 90-93)
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGBA8888;
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Load the source image (from original lines 95-99)
    // Resource file located in tests/resources/scale/
    if (vg_lite_load_raw(&source_, "resources/scale/circle.raw") != 0) {
        GTEST_SKIP() << "SKIPPED: resources/scale/circle.raw file not found";
    }
    
    // Test all scale factors (from original lines 101-108)
    for (int i = 0; i < kScaleFactorCount; i++) {
        error_ = BlitOperation(&source_, &buffer_, kScaleFactors[i]);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "BlitOperation failed at scale=" << kScaleFactors[i];
    }
}

} // anonymous namespace
