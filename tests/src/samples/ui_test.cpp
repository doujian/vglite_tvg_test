/*
 * ui_test.cpp - UI icon grid test
 * 
 * Migrated from VSI_CTS/samples/ui/main.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\ui\main.c
 * 
 * This test demonstrates UI icon grid rendering with highlight overlay.
 * NOTE: Requires external resource files (icons/*.raw).
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"

namespace {

class UiTest : public VGLiteTest {
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
        if (buffer_.handle != nullptr) {
            vg_lite_free(&buffer_);
        }
        vg_lite_close();
    }
    
    vg_lite_buffer_t buffer_;
    
    static constexpr int FB_WIDTH = 320;
    static constexpr int FB_HEIGHT = 480;
    static constexpr int ICON_SIZE = 128;
    static constexpr int ICON_COUNT = 6;
    
    static char path_data[];
    static vg_lite_path_t path_;
};

char UiTest::path_data[] = {
    2,  0,  0,          // moveto    0,  0
    4, 10,  0,          // lineto   10,  0
    4, 10, 10,          // lineto   10, 10
    4,  0, 10,          // lineto    0, 10
    0,                  // end
};

vg_lite_path_t UiTest::path_ = {
    { -10, -10, 10, 10 },
    VG_LITE_HIGH,
    VG_LITE_S8,
    {0},
    sizeof(path_data),
    path_data,
    1
};

TEST_F(UiTest, DISABLED_Ui) {
    GTEST_SKIP() << "SKIPPED: Requires external resource files (icons/*.raw) not available in test environment";
    
    /* Original test code preserved below for reference */
    /*
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_filter_t filter = VG_LITE_FILTER_POINT;
    vg_lite_matrix_t icon_matrix, highlight_matrix;
    int gap_x, gap_y, icon_id;
    
    error = vg_lite_init(FB_WIDTH, FB_HEIGHT);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    buffer_.width = FB_WIDTH;
    buffer_.height = FB_HEIGHT;
    buffer_.format = VG_LITE_RGB565;
    
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    // Load icons would require:
    // vg_lite_load_raw(&icons[i], "icons/1.raw") etc.
    
    error = vg_lite_clear(&buffer_, NULL, 0xFFFF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    // Draw 6 icons in 3x2 grid
    gap_x = (FB_WIDTH - ICON_SIZE * 3) / 4;
    gap_y = (FB_HEIGHT - ICON_SIZE * 2) / 3;
    
    int icon_pos[6][2];
    icon_id = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            icon_pos[i * 3 + j][0] = gap_x * (j + 1) + j * ICON_SIZE;
            icon_pos[i * 3 + j][1] = gap_y * (i + 1) + i * ICON_SIZE;
            
            vg_lite_identity(&icon_matrix);
            vg_lite_translate(icon_pos[icon_id][0], icon_pos[icon_id][1], &icon_matrix);
            vg_lite_scale((float)ICON_SIZE / icons[icon_id].width, 
                          (float)ICON_SIZE / icons[icon_id].height, &icon_matrix);
            
            error = vg_lite_blit(&buffer_, &icons[icon_id], &icon_matrix, 
                                 VG_LITE_BLEND_SRC_OVER, 0, filter);
            ASSERT_EQ(error, VG_LITE_SUCCESS);
            icon_id++;
        }
    }
    
    // Draw highlight rectangle
    vg_lite_identity(&highlight_matrix);
    vg_lite_translate(icon_pos[2][0], icon_pos[2][1], &highlight_matrix);
    vg_lite_scale(ICON_SIZE / 10.0f, ICON_SIZE / 10.0f, &highlight_matrix);
    
    error = vg_lite_draw(&buffer_, &path_, VG_LITE_FILL_EVEN_ODD, &highlight_matrix, 
                         VG_LITE_BLEND_SRC_OVER, 0x22444488);
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    
    error = vg_lite_finish();
    ASSERT_EQ(error, VG_LITE_SUCCESS);
    */
}

} // anonymous namespace
