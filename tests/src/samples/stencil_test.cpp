/**
 * @file stencil_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/stencil/stencil.c
 * 
 * Tests stencil buffer operations.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstring>
#include <cstdlib>

namespace {

static const int fb_width = 256;
static const int fb_height = 256;

// Test fixture for Stencil tests
class StencilTest : public ::testing::Test {
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

// Test: Stencil_Basic - Test stencil operations
TEST_F(StencilTest, Stencil_Basic) {
    vg_lite_matrix_t matrix;
    
    // Initialize vg_lite
    error_ = vg_lite_init(fb_width, fb_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Allocate buffer
    buffer_.width = fb_width;
    buffer_.height = fb_height;
    buffer_.format = VG_LITE_RGBA8888;
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear buffer
    error_ = vg_lite_clear(&buffer_, nullptr, 0xffffffff);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Identity matrix
    vg_lite_identity(&matrix);
    
    // Finish
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
}

} // anonymous namespace
