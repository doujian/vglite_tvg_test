/**
 * Clear Test - Google Test conversion
 * 
 * Original: VGLite_Tests/VSI_CTS/samples/clear/clear.c
 * 
 * Resolution: 256 x 256
 * Format: VG_LITE_RGB565
 * Description: Clear whole buffer with blue first, then clear a sub-rectangle with red.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vglite_test.h"
#include "test_helpers.h"

class ClearTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
    }
};

// Original test: Clear with blue, then clear sub-rectangle with red
TEST_F(ClearTest, Clear) {
    // Allocate 256x256 RGB565 buffer
    buffer_.width = 256;
    buffer_.height = 256;
    buffer_.format = VG_LITE_RGB565;
    ASSERT_VGLITE_SUCCESS(vg_lite_allocate(&buffer_));
    
    // Clear the buffer with blue
    ASSERT_VGLITE_SUCCESS(vg_lite_clear(&buffer_, nullptr, 0xFFFF0000));
    
    // Clear a sub-rectangle (64,64) size 64x64 with red
    vg_lite_rectangle_t rect = { 64, 64, 64, 64 };
    ASSERT_VGLITE_SUCCESS(vg_lite_clear(&buffer_, &rect, 0xFF0000FF));
    
    // Finish rendering
    ASSERT_VGLITE_SUCCESS(vg_lite_finish());
    
    // Note: Original saves to clear.png - image comparison can be added later
    // EXPECT_IMAGE_MATCH(buffer_, "clear/clear");
}
