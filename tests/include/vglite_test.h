#ifndef VGLITE_TEST_H
#define VGLITE_TEST_H

#define VGLITE_TEST_H

#include <gtest/gtest.h>
#include <cstring>
extern "C" {
#include "vg_lite.h"
}
#include "vg_lite.h"

class VGLiteTest : public ::testing::Test {
protected:
    vg_lite_buffer_t buffer_;
    vg_lite_error_t error_;
    int32_t width_;
    int32_t height_;

    void SetUp() override {
        width_ = 256;
        height_ = 256;
        error_ = vg_lite_init(width_, height_);
        ASSERT_EQ(error_, VG_LITE_SUCCESS) << "vg_lite_init failed";
        memset(&buffer_, 0, sizeof(buffer_));
    }

    void TearDown() override {
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        vg_lite_close();
    }

    // Helper to allocate buffer with format
    vg_lite_error_t AllocateBuffer(int32_t width, int32_t height, vg_lite_buffer_format_t format) {
        buffer_.width = width;
        buffer_.height = height;
        buffer_.format = format;
        return vg_lite_allocate(&buffer_);
    }

    // Helper to check for unsupported and skip
    void SkipIfUnsupported(vg_lite_error_t err, const char* func_name) {
        if (err == VG_LITE_NOT_SUPPORT) {
            GTEST_SKIP() << "Function " << func_name << " is not supported by ThorVG adapter";
        }
    }

    // Helper to check VGLite call - skip if unsupported, assert on failure
    void CheckVGLiteCall(vg_lite_error_t err, const char* func_name) {
        if (err == VG_LITE_NOT_SUPPORT) {
            GTEST_SKIP() << func_name << " is not supported";
        }
        EXPECT_EQ(err, VG_LITE_SUCCESS) << func_name << " failed";
    }
};

// Macro for VGLite calls with automatic skip handling
#define VGLITE_CHECK(call) \
    do { \
        vg_lite_error_t _err = call; \
        if (_err == VG_LITE_NOT_SUPPORT) { \
            GTEST_SKIP() << #call << " is not supported"; \
        } \
        EXPECT_EQ(_err, VG_LITE_SUCCESS) << #call << " failed"; \
    } while(0)

// Macro for allocating test buffer
#define VGLITE_ALLOCATE(w, h, fmt) \
    do { \
        buffer_.width = w; \
        buffer_.height = h; \
        buffer_.format = fmt; \
        vg_lite_error_t _err = vg_lite_allocate(&buffer_); \
        ASSERT_EQ(_err, VG_LITE_SUCCESS) << "Failed to allocate buffer"; \
    } while(0)

#endif /* VGLITE_TEST_H */
