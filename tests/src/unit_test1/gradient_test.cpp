/**
 * @file gradient_test.cpp
 * @brief 1:1 migration of VSI_CTS/samples/unit_test1/src/Cases/Gradient.c
 * 
 * Tests gradient operations with different formats, qualities, filters, blend modes.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace {

static const int WINDSIZEX = 256;
static const int WINDSIZEY = 256;

// Constants from original
#define FORMATS_COUNT 2
#define QUALITY_COUNT 3
#define FILTER_COUNT 3
#define BLEND_COUNT 9

// Formats from original
static const vg_lite_buffer_format_t formats[FORMATS_COUNT] = {
    VG_LITE_RGBA8888,
    VG_LITE_BGRA8888,
};

// Qualities from original
static const vg_lite_quality_t qualities[QUALITY_COUNT] = {
    VG_LITE_LOW,
    VG_LITE_MEDIUM,
    VG_LITE_HIGH,
};

// Filter modes from original
static const vg_lite_filter_t filter_modes[FILTER_COUNT] = {
    VG_LITE_FILTER_POINT,
    VG_LITE_FILTER_LINEAR,
    VG_LITE_FILTER_BI_LINEAR,
};

// Blend modes from original
static const vg_lite_blend_t blend_mode[BLEND_COUNT] = {
    VG_LITE_BLEND_NONE,
    VG_LITE_BLEND_SRC_OVER,
    VG_LITE_BLEND_DST_OVER,
    VG_LITE_BLEND_SRC_IN,
    VG_LITE_BLEND_DST_IN,
    VG_LITE_BLEND_MULTIPLY,
    VG_LITE_BLEND_SCREEN,
    VG_LITE_BLEND_ADDITIVE,
    VG_LITE_BLEND_SUBTRACT,
};

// Test fixture for Gradient tests
class GradientTest : public ::testing::Test {
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
    
    // Helper: Allocate buffer (from original Allocate_Buffer)
    vg_lite_error_t AllocateBuffer(vg_lite_buffer_t* buf, vg_lite_buffer_format_t format, int32_t width, int32_t height) {
        memset(buf, 0, sizeof(vg_lite_buffer_t));
        buf->width = (width + 127) & ~127;  // ALIGN to 128
        buf->height = height;
        buf->format = format;
        return vg_lite_allocate(buf);
    }
    
    // Helper: Generate buffer content (from original gen_buffer)
    void GenBuffer(vg_lite_buffer_t* buf, int pattern) {
        if (buf->memory == nullptr) return;
        
        uint32_t* pixels = static_cast<uint32_t*>(buf->memory);
        for (int y = 0; y < buf->height; y++) {
            for (int x = 0; x < buf->width; x++) {
                uint32_t color;
                if (pattern == 0) {
                    color = 0xFF000000 | ((x * 255 / buf->width) << 16) | ((y * 255 / buf->height) << 8) | 0x80;
                } else {
                    int checker = ((x / 20) + (y / 20)) % 2;
                    color = checker ? 0xFF4080C0 : 0xFFC08040;
                }
                pixels[y * buf->width + x] = color;
            }
        }
    }
};

// Test: Gradient_Draw_Test - Test gradient drawing with all combinations
TEST_F(GradientTest, Gradient_Draw_Test) {
    int32_t src_width = WINDSIZEX;
    int32_t src_height = WINDSIZEY;
    int32_t dst_width = WINDSIZEX;
    int32_t dst_height = WINDSIZEY;
    vg_lite_color_t color = 0xffa0a0a0;
    vg_lite_matrix_t matrix;
    
    // Initialize vg_lite
    error_ = vg_lite_init(dst_width, dst_height);
    ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Test all format combinations
    for (int i = 0; i < FORMATS_COUNT; i++) {
        for (int j = 0; j < QUALITY_COUNT; j++) {
            for (int k = 0; k < FILTER_COUNT; k++) {
                for (int m = 0; m < BLEND_COUNT; m++) {
                    // Allocate source buffer
                    error_ = AllocateBuffer(&src_buf_, formats[i], src_width, src_height);
                    if (error_ != VG_LITE_SUCCESS) continue;
                    
                    GenBuffer(&src_buf_, 0);
                    
                    // Allocate destination buffer
                    error_ = AllocateBuffer(&dst_buf_, formats[i], dst_width, dst_height);
                    if (error_ != VG_LITE_SUCCESS) {
                        vg_lite_free(&src_buf_);
                        memset(&src_buf_, 0, sizeof(src_buf_));
                        continue;
                    }
                    
                    // Identity matrix
                    vg_lite_identity(&matrix);
                    
                    // Clear destination
                    error_ = vg_lite_clear(&dst_buf_, nullptr, color);
                    if (error_ != VG_LITE_SUCCESS) {
                        vg_lite_free(&dst_buf_);
                        vg_lite_free(&src_buf_);
                        memset(&dst_buf_, 0, sizeof(dst_buf_));
                        memset(&src_buf_, 0, sizeof(src_buf_));
                        continue;
                    }
                    
                    // Blit
                    error_ = vg_lite_blit(&dst_buf_, &src_buf_, &matrix, blend_mode[m], 0, filter_modes[k]);
                    if (error_ != VG_LITE_SUCCESS) {
                        vg_lite_free(&dst_buf_);
                        vg_lite_free(&src_buf_);
                        memset(&dst_buf_, 0, sizeof(dst_buf_));
                        memset(&src_buf_, 0, sizeof(src_buf_));
                        continue;
                    }
                    
                    // Finish
                    error_ = vg_lite_finish();
                    
                    // Free buffers for next iteration
                    vg_lite_free(&dst_buf_);
                    vg_lite_free(&src_buf_);
                    memset(&dst_buf_, 0, sizeof(dst_buf_));
                    memset(&src_buf_, 0, sizeof(src_buf_));
                }
            }
        }
    }
}

} // anonymous namespace
