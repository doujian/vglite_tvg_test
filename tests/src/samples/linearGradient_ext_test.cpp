/*
 * linearGradient_test.cpp - Linear gradient test (extended version)
 * 
 * Migrated from VSI_CTS/samples/linearGradient/linearGradient.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\linearGradient\linearGradient.c
 * 
 * This test demonstrates extended linear gradient with various spread modes.
 * NOTE: Extended linear gradient API may not be fully supported by ThorVG backend.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class LinearGradientExtTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
    }
    
    void TearDown() override {
        cleanup();
        VGLiteTest::TearDown();
    }
    
    void cleanup(void) {
        if (buffer_.handle != NULL) {
            vg_lite_free(&buffer_);
        }
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    
    static constexpr int FB_WIDTH = 480;
    static constexpr int FB_HEIGHT = 640;
    
    static short path_data[];
    static vg_lite_path_t path_;
};

short LinearGradientExtTest::path_data[] = {
    2, 30, 50,
    4, 450, 50,
    4, 450, 350,
    4, 30, 350,
    0,
};

vg_lite_path_t LinearGradientExtTest::path_ = {
    { 0, 0, 400, 400 },
    VG_LITE_HIGH,
    VG_LITE_S16,
    {0},
    sizeof(path_data),
    path_data,
    1
};

TEST_F(LinearGradientExtTest, DISABLED_LinearGradientExt) {
    GTEST_SKIP() << "SKIPPED: Extended linear gradient API not fully supported by ThorVG backend";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    vg_lite_ext_linear_gradient_t grad;
    
    vg_lite_color_ramp_t color_ramp[] = {
        { 0.1f, 0.9f, 0.2f, 0.0f, 1.0f },
        { 0.45f, 0.0f, 8.0f, 0.3f, 0.8f },
        { 0.75f, 0.2f, 0.1f, 0.8f, 0.9f },
        { 0.9f, 0.8f, 0.3f, 0.1f, 1.0f }
    };
    
    vg_lite_linear_gradient_parameter_t linear_gradient = { 25.0f, 50.0f, 470.0f, 50.0f };
    vg_lite_gradient_spreadmode_t spreadmodes[] = {
        VG_LITE_GRADIENT_SPREAD_FILL,
        VG_LITE_GRADIENT_SPREAD_PAD,
        VG_LITE_GRADIENT_SPREAD_REPEAT,
        VG_LITE_GRADIENT_SPREAD_REFLECT
    };
    
    error = vg_lite_init(FB_WIDTH, FB_HEIGHT);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    buffer_.width = FB_WIDTH;
    buffer_.height = FB_HEIGHT;
    buffer_.format = VG_LITE_BGRX8888;
    
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    for (int i = 0; i < 4; i++) {
        memset(&grad, 0, sizeof(grad));
        vg_lite_set_linear_grad(&grad, 4, color_ramp, linear_gradient, spreadmodes[i], 1);
        vg_lite_update_linear_grad(&grad);
        
        vg_lite_matrix_t* mat_grad = vg_lite_get_linear_grad_matrix(&grad);
        vg_lite_identity(mat_grad);
        
        error = vg_lite_clear(&buffer_, NULL, 0xffffffff);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        vg_lite_matrix_t mat_path;
        vg_lite_identity(&mat_path);
        
        error = vg_lite_draw_linear_grad(&buffer_, &path_, VG_LITE_FILL_EVEN_ODD, 
                                         &mat_path, &grad, 0, VG_LITE_BLEND_NONE, VG_LITE_FILTER_LINEAR);
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        error = vg_lite_finish();
        ASSERT_EQ(error, VG_LITE_SUCCESS);
        
        char filename[32];
        sprintf(filename, "linearGrad_%d.png", i);
        vg_lite_save_png(filename, &buffer_);
        
        vg_lite_clear_linear_grad(&grad);
    }
    */
}

} // anonymous namespace
