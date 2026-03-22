/*
 * rgba8_etc2_test.cpp - RGBA8 ETC2 compression test
 * 
 * Migrated from VSI_CTS/samples/rgba8_etc2/rgba8_etc2.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\rgba8_etc2\rgba8_etc2.c
 * 
 * This test demonstrates PKM/ETC2 compressed texture loading.
 * NOTE: Requires external PKM file and ETC2 decompression feature.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class Rgba8Etc2Test : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
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
        if (image_.handle != nullptr) {
            vg_lite_free(&image_);
        }
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    vg_lite_buffer_t image_;
    
    static constexpr int FB_WIDTH = 320;
    static constexpr int FB_HEIGHT = 480;
};

TEST_F(Rgba8Etc2Test, DISABLED_Rgba8Etc2) {
    GTEST_SKIP() << "SKIPPED: Requires landscape.pkm resource file and ETC2 compression feature not supported by ThorVG backend";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_matrix_t matrix;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    
    error = vg_lite_init(32, 32);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_RGBA8_ETC2_EAC)) {
        GTEST_SKIP() << "RGBA8 ETC2 EAC feature not supported";
    }
    
    // Load PKM file would be required:
    // vg_lite_load_pkm_info_to_buffer(&image_, "landscape.pkm")
    // vg_lite_load_pkm(&image_, "landscape.pkm")
    
    buffer_.width = FB_WIDTH;
    buffer_.height = FB_HEIGHT;
    buffer_.format = VG_LITE_RGB565;
    
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    error = vg_lite_clear(&buffer_, NULL, 0xFFFF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    vg_lite_identity(&matrix);
    vg_lite_translate(FB_WIDTH / 2.0f, FB_HEIGHT / 2.0f, &matrix);
    vg_lite_rotate(33.0f, &matrix);
    vg_lite_translate(FB_WIDTH / -2.0f, FB_HEIGHT / -2.0f, &matrix);
    vg_lite_scale((float)FB_WIDTH / image_.width, (float)FB_HEIGHT / image_.height, &matrix);
    
    error = vg_lite_blit(&buffer_, &image_, &matrix, VG_LITE_BLEND_NONE, 0, filter);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    error = vg_lite_finish();
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    */
}

} // anonymous namespace
