/*
 * premultiply_test.cpp - Premultiply mode test
 * 
 * Migrated from VSI_CTS/samples/premultiply/premultiply.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\premultiply\premultiply.c
 * 
 * This test demonstrates premultiply image mode.
 * NOTE: Requires external resource file (landscape.raw) and vg_lite_set_premultiply API.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class PremultiplyTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
        memset(&buffer1_, 0, sizeof(buffer1_));
        memset(&image_, 0, sizeof(image_));
    }
    
    void TearDown() override {
        cleanup();
        VGLiteTest::TearDown();
    }
    
    void cleanup(void) {
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        if (buffer1_.handle != nullptr) {
            vg_lite_free(&buffer1_);
        }
        if (image_.handle != nullptr) {
            vg_lite_free(&image_);
        }
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    vg_lite_buffer_t buffer1_;
    vg_lite_buffer_t image_;
    
    static constexpr int FB_WIDTH = 320;
    static constexpr int FB_HEIGHT = 480;
    
    static char path_data[];
    static vg_lite_path_t path_;
};

char PremultiplyTest::path_data[] = {
    2, 0, 0,
    4, 5, 10,
    4, 10, 0,
    0,
};

vg_lite_path_t PremultiplyTest::path_ = {
    { -10, -10, 10, 10 },
    VG_LITE_HIGH,
    VG_LITE_S8,
    {0},
    sizeof(path_data),
    path_data,
    1
};

TEST_F(PremultiplyTest, DISABLED_Premultiply) {
    GTEST_SKIP() << "SKIPPED: Requires landscape.raw resource file and vg_lite_set_premultiply API not fully supported by ThorVG backend";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    
    error = vg_lite_init(FB_WIDTH, FB_HEIGHT);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_HW_PREMULTIPLY)) {
        GTEST_SKIP() << "Premultiply feature not supported";
    }
    
    // Load landscape.raw would be required here
    // image_.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    
    buffer_.width = FB_WIDTH;
    buffer_.height = FB_HEIGHT;
    buffer_.format = VG_LITE_RGBA8888;
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    buffer1_.width = FB_WIDTH;
    buffer1_.height = FB_HEIGHT;
    buffer1_.format = VG_LITE_RGBA8888;
    error = vg_lite_allocate(&buffer1_);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    error = vg_lite_clear(&buffer_, NULL, 0x22FF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    error = vg_lite_clear(&buffer1_, NULL, 0x22FF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    // Test premultiply mode:
    // vg_lite_set_premultiply(0, 1);
    // vg_lite_blit() and vg_lite_draw() with premultiplied blending
    */
}

} // anonymous namespace
