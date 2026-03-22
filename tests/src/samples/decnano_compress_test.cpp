/*
 * decnano_compress_test.cpp - DecNano compression test
 * 
 * Migrated from VSI_CTS/samples/decnano_compress/decnano_compress.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\decnano_compress\decnano_compress.c
 * 
 * This test demonstrates DecNano compression and decompression.
 * NOTE: DecNano compression APIs not supported by ThorVG backend.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class DecnanoCompressTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
        memset(&buffer1_, 0, sizeof(buffer1_));
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
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    vg_lite_buffer_t buffer1_;
    
    static constexpr int FB_WIDTH = 256;
    static constexpr int FB_HEIGHT = 256;
};

TEST_F(DecnanoCompressTest, DISABLED_DecnanoCompress) {
    GTEST_SKIP() << "SKIPPED: DecNano compression APIs (vg_lite_save/load_decnano_compressd_data) not supported by ThorVG backend";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_rectangle_t rect = { 64, 64, 64, 64 };
    vg_lite_matrix_t matrix;
    vg_lite_blend_t blend = VG_LITE_BLEND_SRC_OVER;
    
    vg_lite_compress_mode_t compress_modes[] = {
        VG_LITE_DEC_NON_SAMPLE,
        VG_LITE_DEC_HSAMPLE,
        VG_LITE_DEC_HV_SAMPLE,
    };
    
    vg_lite_buffer_format_t compress_formats[] = {
        VG_LITE_BGRX8888, 
        VG_LITE_RGBX8888,
        VG_LITE_BGRA8888,
        VG_LITE_RGBA8888,
        VG_LITE_RGB888,
        VG_LITE_BGR888,
    };
    
    error = vg_lite_init(0, 0);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    if (!vg_lite_query_feature(gcFEATURE_BIT_VG_DEC_COMPRESS)) {
        GTEST_SKIP() << "DecNano compress feature not supported";
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 6; j++) {
            buffer_.width = FB_WIDTH;
            buffer_.height = FB_HEIGHT;
            buffer_.format = compress_formats[j];
            buffer_.compress_mode = compress_modes[i];
            
            if (compress_modes[i] == VG_LITE_DEC_HV_SAMPLE)
                buffer_.tiled = VG_LITE_TILED;
            
            error = vg_lite_allocate(&buffer_);
            ASSERT_EQ(error, VG_LITE_SUCCESS);
            
            error = vg_lite_clear(&buffer_, NULL, 0xFFFF0000);
            ASSERT_EQ(error, VG_LITE_SUCCESS);
            
            error = vg_lite_clear(&buffer_, &rect, 0xFF0000FF);
            ASSERT_EQ(error, VG_LITE_SUCCESS);
            
            error = vg_lite_finish();
            ASSERT_EQ(error, VG_LITE_SUCCESS);
            
            // Save and load compressed data would be tested here
            // vg_lite_save_decnano_compressd_data()
            // vg_lite_load_decnano_compressd_data()
        }
    }
    */
}

} // anonymous namespace
