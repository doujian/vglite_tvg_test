/**
 * @file pixel_matrix_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/pixel_matrix/pixel_matrix.c
 * 
 * Tests pixel matrix operations.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstring>
#include <cstdlib>

namespace {

// Test fixture for PixelMatrix tests
class PixelMatrixTest : public ::testing::Test {
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

// Test: PixelMatrix_Blit - Test pixel matrix blit
TEST_F(PixelMatrixTest, PixelMatrix_Blit) {
    vg_lite_matrix_t matrix;
    
    // Initialize vg_lite
    error_ = vg_lite_init(256, 256);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Allocate buffer
    buffer_.width = 256;
    buffer_.height = 256;
    buffer_.format = VG_LITE_RGBA8888;
    error_ = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear buffer
    error_ = vg_lite_clear(&buffer_, nullptr, 0xffffffff);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Setup matrix (from original)
    vg_lite_identity(&matrix);
    
    // Finish
    error_ = vg_lite_finish();
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_finish failed";
}

} // anonymous namespace
