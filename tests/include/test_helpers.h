#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "image_compare.h"

// Skip test if function returns VG_LITE_NOT_SUPPORT
#define SKIP_IF_UNSUPPORTED(call, func_name) \
    do { \
        vg_lite_error_t _err = call; \
        if (_err == VG_LITE_NOT_SUPPORT) { \
            GTEST_SKIP() << func_name << " is not supported by ThorVG adapter"; \
        } \
    } while(0)

// Assert VGLite call succeeds - skip if unsupported, fail if error
#define ASSERT_VGLITE_SUCCESS(call) \
    do { \
        vg_lite_error_t _err = call; \
        if (_err == VG_LITE_NOT_SUPPORT) { \
            GTEST_SKIP() << #call << " is not supported"; \
        } \
        ASSERT_EQ(_err, VG_LITE_SUCCESS) << #call << " failed"; \
    } while(0)

// Expect VGLite call succeeds (non-fatal) - skip if unsupported
#define EXPECT_VGLITE_SUCCESS(call) \
    do { \
        vg_lite_error_t _err = call; \
        if (_err == VG_LITE_NOT_SUPPORT) { \
            GTEST_SKIP() << #call << " is not supported"; \
        } \
        EXPECT_EQ(_err, VG_LITE_SUCCESS) << #call << " failed"; \
    } while(0)

// Compare output buffer with reference image
// Uses 1% tolerance by default
#define EXPECT_IMAGE_MATCH(buffer, ref_name) \
    EXPECT_TRUE(vglite::test::CompareWithReference(&buffer, ref_name, 0.01f)) \
        << "Image does not match reference: " << ref_name

// Compare output buffer with reference image with custom tolerance
#define EXPECT_IMAGE_MATCH_TOLERANCE(buffer, ref_name, tol) \
    EXPECT_TRUE(vglite::test::CompareWithReference(&buffer, ref_name, tol)) \
        << "Image does not match reference (tolerance=" << tol << "): " << ref_name

// Assert buffer allocation succeeds
#define ASSERT_BUFFER_ALLOCATE(buffer, w, h, fmt) \
    do { \
        buffer.width = w; \
        buffer.height = h; \
        buffer.format = fmt; \
        ASSERT_VGLITE_SUCCESS(vg_lite_allocate(&buffer)); \
    } while(0)

// Helper to create a simple test path (rectangle)
#define CREATE_RECT_PATH(path_data, x, y, w, h) \
    uint8_t path_data[] = { \
        2, (uint8_t)(x), (uint8_t)((x) >> 8), (uint8_t)((x) >> 16), (uint8_t)((x) >> 24), \
           (uint8_t)(y), (uint8_t)((y) >> 8), (uint8_t)((y) >> 16), (uint8_t)((y) >> 24), \
        4, (uint8_t)(w), (uint8_t)((w) >> 8), (uint8_t)((w) >> 16), (uint8_t)((w) >> 24), \
           0, 0, 0, 0, \
        4, 0, 0, 0, 0, \
           (uint8_t)(h), (uint8_t)((h) >> 8), (uint8_t)((h) >> 16), (uint8_t)((h) >> 24), \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        0 \
    }

// Common color values in BGRA format
#define COLOR_RED     0xFF0000FF
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFFFF0000
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_BLACK   0xFF000000
#define COLOR_YELLOW  0xFF00FFFF
#define COLOR_CYAN    0xFFFFFF00
#define COLOR_MAGENTA 0xFFFF00FF

#endif /* TEST_HELPERS_H */
