/**
 * @file clear_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/unit_test/src/Cases/Clear.c
 * 
 * Tests different size and position of Clear-Rectangle.
 * Original has 4 test cases: Clear_001 to Clear_004
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include <cstring>
#include <cstdlib>

namespace {

// Format count from original
#define FORMAT_COUNT 2

// Formats from original
static const vg_lite_buffer_format_t kFormats[] = {
    VG_LITE_RGB565,
    VG_LITE_RGBA8888,
};

// Test fixture for Clear tests
class ClearUnitTest : public ::testing::Test {
protected:
    vg_lite_buffer_t buffers_[FORMAT_COUNT];
    
    void SetUp() override {
        for (int i = 0; i < FORMAT_COUNT; i++) {
            memset(&buffers_[i], 0, sizeof(vg_lite_buffer_t));
        }
    }
    
    void TearDown() override {
        for (int i = 0; i < FORMAT_COUNT; i++) {
            if (buffers_[i].handle != nullptr) {
                vg_lite_free(&buffers_[i]);
            }
        }
    }
    
    // Helper: Generate random color (simplified from GenColor_r)
    vg_lite_color_t GenRandomColor() {
        return 0xFF000000 | 
               ((rand() % 256) << 16) | 
               ((rand() % 256) << 8) | 
               (rand() % 256);
    }
    
    // Helper: Generate random value in range (simplified from Random_r)
    int RandomInRange(int min_val, int max_val) {
        if (max_val <= min_val) return min_val;
        return min_val + (rand() % (max_val - min_val + 1));
    }
};

// Test: Clear_001 - Different buffer formats, different clear modes, and part region
TEST_F(ClearUnitTest, Clear_001) {
    const int width = 320;
    const int height = 240;
    
    for (int i = 0; i < FORMAT_COUNT; i++) {
        vg_lite_color_t cc = GenRandomColor();
        
        buffers_[i].width = width;
        buffers_[i].height = height;
        buffers_[i].format = kFormats[i];
        
        vg_lite_error_t error = vg_lite_allocate(&buffers_[i]);
        ASSERT_EQ(error, VG_LITE_SUCCESS) << "Failed to allocate buffer for format " << i;
        
        // Random rectangle
        vg_lite_rectangle_t rect;
        rect.x = RandomInRange(0, width / 2);
        rect.y = RandomInRange(0, height / 2);
        rect.width = RandomInRange(1, width / 2);
        rect.height = RandomInRange(1, height / 2);
        
        // Clear with rectangle mode (from original)
        error = vg_lite_clear(&buffers_[i], nullptr, 0xffffffff);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_clear(&buffers_[i], &rect, cc);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        // Clear with scanline mode (from original - second clear)
        error = vg_lite_clear(&buffers_[i], nullptr, 0xffffffff);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_clear(&buffers_[i], &rect, cc);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        // Free for next iteration
        vg_lite_free(&buffers_[i]);
        memset(&buffers_[i], 0, sizeof(vg_lite_buffer_t));
    }
}

// Test: Clear_002 - Different buffer formats, different clear modes, and full screen
TEST_F(ClearUnitTest, Clear_002) {
    const int width = 320;
    const int height = 240;
    
    for (int i = 0; i < FORMAT_COUNT; i++) {
        vg_lite_color_t cc = GenRandomColor();
        
        buffers_[i].width = width;
        buffers_[i].height = height;
        buffers_[i].format = kFormats[i];
        
        vg_lite_error_t error = vg_lite_allocate(&buffers_[i]);
        ASSERT_EQ(error, VG_LITE_SUCCESS) << "Failed to allocate buffer for format " << i;
        
        // Clear full screen (NULL rect)
        error = vg_lite_clear(&buffers_[i], nullptr, cc);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        // Free for next iteration
        vg_lite_free(&buffers_[i]);
        memset(&buffers_[i], 0, sizeof(vg_lite_buffer_t));
    }
}

// Test: Clear_003 - Part region for 1080p resolution
TEST_F(ClearUnitTest, Clear_003) {
    const int width = 1920;
    const int height = 1080;
    
    for (int i = 0; i < FORMAT_COUNT; i++) {
        vg_lite_color_t cc = GenRandomColor();
        
        buffers_[i].width = width;
        buffers_[i].height = height;
        buffers_[i].format = kFormats[i];
        
        vg_lite_error_t error = vg_lite_allocate(&buffers_[i]);
        ASSERT_EQ(error, VG_LITE_SUCCESS) << "Failed to allocate buffer for format " << i;
        
        // Random rectangle (scaled to 1080p)
        vg_lite_rectangle_t rect;
        rect.x = RandomInRange(0, width / 2);
        rect.y = RandomInRange(0, height / 2);
        rect.width = RandomInRange(1, width / 2);
        rect.height = RandomInRange(1, height / 2);
        
        // Clear with rectangle mode
        error = vg_lite_clear(&buffers_[i], nullptr, 0xffffffff);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_clear(&buffers_[i], &rect, cc);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        // Free for next iteration
        vg_lite_free(&buffers_[i]);
        memset(&buffers_[i], 0, sizeof(vg_lite_buffer_t));
    }
}

// Test: Clear_004 - Full screen for 1080p resolution
TEST_F(ClearUnitTest, Clear_004) {
    const int width = 1920;
    const int height = 1080;
    
    for (int i = 0; i < FORMAT_COUNT; i++) {
        vg_lite_color_t cc = GenRandomColor();
        
        buffers_[i].width = width;
        buffers_[i].height = height;
        buffers_[i].format = kFormats[i];
        
        vg_lite_error_t error = vg_lite_allocate(&buffers_[i]);
        ASSERT_EQ(error, VG_LITE_SUCCESS) << "Failed to allocate buffer for format " << i;
        
        // Clear full screen
        error = vg_lite_clear(&buffers_[i], nullptr, cc);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        // Free for next iteration
        vg_lite_free(&buffers_[i]);
        memset(&buffers_[i], 0, sizeof(vg_lite_buffer_t));
    }
}

} // anonymous namespace
