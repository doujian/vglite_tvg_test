/**
 * @file pixels_align_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/16pixels_align/16pixels_align.c
 * 
 * Resolution: 2 x 2
 * Format: VG_LITE_RGBA8888
 * Description: Read 4 pixels data into srcbuf, then blit to dstbuf.
 * Shows that dstbuf's width does not need to be aligned to 16 pixels.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstring>
#include <cstdlib>

namespace {

// Test fixture for 16pixels_align tests
class PixelsAlignTest : public ::testing::Test {
protected:
    vg_lite_buffer_t src_buf_;
    vg_lite_buffer_t dst_buf_;
    vg_lite_error_t error_;
    
    void SetUp() override {
        memset(&src_buf_, 0, sizeof(src_buf_));
        memset(&dst_buf_, 0, sizeof(dst_buf_));
        error_ = VG_LITE_SUCCESS;
    }
    
    void TearDown() override {
        if (src_buf_.handle != nullptr) {
            vg_lite_free(&src_buf_);
        }
        if (dst_buf_.handle != nullptr) {
            vg_lite_free(&dst_buf_);
        }
        vg_lite_close();
    }
};

// Test: PixelsAlign_Blit - Test blit with 2x2 pixel buffer
TEST_F(PixelsAlignTest, PixelsAlign_Blit) {
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    const int fb_width = 2;
    const int fb_height = 2;
    
    // Initialize vg_lite (from original line 51)
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Setup color data (from original line 53)
    const unsigned int color_data[4] = { 0xff000050, 0xff005000, 0xff500000, 0xff505050 };
    
    // Identity matrix (from original line 54)
    vg_lite_identity(&matrix);
    
    // Setup source buffer (from original lines 57-65)
    src_buf_.width = fb_width;
    src_buf_.height = fb_height;
    src_buf_.format = VG_LITE_RGBA8888;
    
    // Check 16pixels align feature (from original line 60-61)
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_16PIXELS_ALIGN)) {
        src_buf_.width = ((src_buf_.width + 15) & ~0xf);
    }
    
    error_ = vg_lite_allocate(&src_buf_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate src failed";
    
    // Fill source buffer (from original lines 63-65)
    memset(src_buf_.memory, 0xffffffff, src_buf_.stride * src_buf_.height);
    memcpy(src_buf_.memory, &color_data[0], 8);
    memcpy(((char*)src_buf_.memory + src_buf_.stride), &color_data[2], 8);
    
    // Setup destination buffer (from original lines 67-72)
    dst_buf_.width = fb_width;
    dst_buf_.height = fb_height;
    dst_buf_.format = VG_LITE_RGBA8888;
    
    if (vg_lite_query_feature(gcFEATURE_BIT_VG_16PIXELS_ALIGN)) {
        dst_buf_.width = ((dst_buf_.width + 15) & ~0xf);
    }
    
    error_ = vg_lite_allocate(&dst_buf_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate dst failed";
    
    // Blit (from original line 74)
    error_ = vg_lite_blit(&dst_buf_, &src_buf_, &matrix, VG_LITE_BLEND_NONE, 0, filter);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_blit failed";
    
    // Finish (from original line 75)
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
}

} // anonymous namespace
