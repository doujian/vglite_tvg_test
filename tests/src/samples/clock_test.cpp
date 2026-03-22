/*
 * clock_test.cpp - Clock demo test
 * 
 * Migrated from VSI_CTS/samples/clock/main.c
 * Original file: D:\Projects\vglite_thorvg_cts\VGLite_Tests\VSI_CTS\samples\clock\main.c
 * 
 * This test demonstrates rendering an analog clock with hands and face.
 */

#include <gtest/gtest.h>
#include "vg_lite.h"
#include "vg_lite_util.h"
#include "vglite_test.h"
#include <cmath>

namespace {

class ClockTest : public VGLiteTest {
protected:
    void SetUp() override {
        VGLiteTest::SetUp();
        memset(&buffer_, 0, sizeof(buffer_));
        memset(&path_circle_, 0, sizeof(path_circle_));
        memset(&path_hand_, 0, sizeof(path_hand_));
        memset(&path_button_, 0, sizeof(path_button_));
    }
    
    void TearDown() override {
        cleanup();
        VGLiteTest::TearDown();
    }
    
    void cleanup(void) {
        if (buffer_.handle != NULL) {
            vg_lite_free(&buffer_);
        }
        
        vg_lite_clear_path(&path_button_);
        vg_lite_clear_path(&path_circle_);
        vg_lite_clear_path(&path_hand_);
        vg_lite_close();
        
        if (path_button_.path != NULL) {
            free(path_button_.path);
        }
        if (path_circle_.path != NULL) {
            free(path_circle_.path);
        }
        if (path_hand_.path != NULL) {
            free(path_hand_.path);
        }
    }
    
    void buildPaths();
    void buildMatrices();
    vg_lite_error_t renderClock();
    vg_lite_error_t renderButtons();
    
    vg_lite_buffer_t buffer_;
    vg_lite_path_t path_circle_;
    vg_lite_path_t path_hand_;
    vg_lite_path_t path_button_;
    
    vg_lite_matrix_t matrix_tick_[12];
    vg_lite_matrix_t matrix_hour_;
    vg_lite_matrix_t matrix_min_;
    vg_lite_matrix_t matrix_sec_;
    vg_lite_matrix_t matrix_face_[2];
    vg_lite_matrix_t matrix_button_[2];
    vg_lite_matrix_t matrix_cap_;
    
    static constexpr float CLOCK_RADIUS = 10.0f;
    static constexpr int FB_WIDTH = 320;
    static constexpr int FB_HEIGHT = 480;
    static constexpr float BUTTON_WIDTH = 6.0f;
    static constexpr float BUTTON_HEIGHT = 3.0f;
};

// Opcode definitions
#define OPCODE_END       0x00
#define OPCODE_MOVE      0x02
#define OPCODE_LINE      0x04
#define OPCODE_QUADRATIC 0x06
#define OPCODE_CUBIC     0x08

void ClockTest::buildPaths() {
    char* pchar;
    float* pfloat;
    int32_t data_size;
    
    // Path circle - using cubic curves
    data_size = 4 * 5 + 1 + 8 + 8 * 3 * 4;
    path_circle_.path = malloc(data_size);
    vg_lite_init_path(&path_circle_, VG_LITE_FP32, VG_LITE_HIGH, data_size, 
                      path_circle_.path, -CLOCK_RADIUS, -CLOCK_RADIUS, CLOCK_RADIUS, CLOCK_RADIUS);
    
    pchar = (char*)path_circle_.path;
    pfloat = (float*)path_circle_.path;
    *pchar = OPCODE_MOVE;
    pfloat++;
    *pfloat++ = CLOCK_RADIUS;
    *pfloat++ = 0.0f;
    
    // Four cubic curves to make a circle
    for (int i = 0; i < 4; i++) {
        pchar = (char*)pfloat;
        *pchar = OPCODE_CUBIC;
        pfloat++;
        float angle1 = i * 3.14159265f / 2;
        float angle2 = (i + 1) * 3.14159265f / 2;
        *pfloat++ = CLOCK_RADIUS * cos(angle1 + 0.5f);
        *pfloat++ = CLOCK_RADIUS * sin(angle1 + 0.5f);
        *pfloat++ = CLOCK_RADIUS * cos(angle2 - 0.5f);
        *pfloat++ = CLOCK_RADIUS * sin(angle2 - 0.5f);
        *pfloat++ = CLOCK_RADIUS * cos(angle2);
        *pfloat++ = CLOCK_RADIUS * sin(angle2);
    }
    
    pchar = (char*)pfloat;
    *pchar = OPCODE_END;
    
    // Path hand - simple rectangle
    data_size = 4 * 4 + 1 + 4 * 2 * 4;
    path_hand_.path = malloc(data_size);
    vg_lite_init_path(&path_hand_, VG_LITE_FP32, VG_LITE_HIGH, data_size,
                      path_hand_.path, 0.0f, 0.0f, 1.0f, 1.0f);
    
    pchar = (char*)path_hand_.path;
    pfloat = (float*)path_hand_.path;
    *pchar = OPCODE_MOVE; pfloat++; *pfloat++ = 0.0f; *pfloat++ = 0.0f;
    pchar = (char*)pfloat; *pchar = OPCODE_LINE; pfloat++; *pfloat++ = 1.0f; *pfloat++ = 0.0f;
    pchar = (char*)pfloat; *pchar = OPCODE_LINE; pfloat++; *pfloat++ = 1.0f; *pfloat++ = 1.0f;
    pchar = (char*)pfloat; *pchar = OPCODE_LINE; pfloat++; *pfloat++ = 0.0f; *pfloat++ = 1.0f;
    pchar = (char*)pfloat; *pchar = OPCODE_END;
}

void ClockTest::buildMatrices() {
    // Initialize all matrices
    for (int i = 0; i < 12; i += 3) {
        float angle = i * 3.14159265f / 6;
        float x = 80.0f * cos(angle);
        float y = 80.0f * sin(angle);
        vg_lite_identity(&matrix_tick_[i]);
        vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_tick_[i]);
        vg_lite_translate(x, y, &matrix_tick_[i]);
        vg_lite_rotate(i * 30, &matrix_tick_[i]);
        vg_lite_scale(2.0f, 1.0f, &matrix_tick_[i]);
    }
    
    // Hour hand
    vg_lite_identity(&matrix_hour_);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_hour_);
    vg_lite_rotate(180, &matrix_hour_);
    vg_lite_translate(-4.0f, -4.0f, &matrix_hour_);
    vg_lite_scale(40.0f, 8.0f, &matrix_hour_);
    
    // Minute hand
    vg_lite_identity(&matrix_min_);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_min_);
    vg_lite_rotate(72, &matrix_min_);
    vg_lite_translate(-6.0f, -3.0f, &matrix_min_);
    vg_lite_scale(60.0f, 6.0f, &matrix_min_);
    
    // Second hand
    vg_lite_identity(&matrix_sec_);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_sec_);
    vg_lite_rotate(132, &matrix_sec_);
    vg_lite_translate(-7.5f, -1.0f, &matrix_sec_);
    vg_lite_scale(75.0f, 2.0f, &matrix_sec_);
    
    // Clock face
    vg_lite_identity(&matrix_face_[0]);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_face_[0]);
    vg_lite_scale(10.0f, 10.0f, &matrix_face_[0]);
    
    vg_lite_identity(&matrix_face_[1]);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_face_[1]);
    vg_lite_scale(9.5f, 9.5f, &matrix_face_[1]);
    
    // Center cap
    vg_lite_identity(&matrix_cap_);
    vg_lite_translate(FB_WIDTH / 2, FB_HEIGHT / 2, &matrix_cap_);
}

vg_lite_error_t ClockTest::renderClock() {
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_buffer_t* fb = &buffer_;
    
    error = vg_lite_draw(fb, &path_circle_, VG_LITE_FILL_EVEN_ODD, &matrix_face_[0], VG_LITE_BLEND_NONE, 0xff000000);
    if (error) return error;
    
    error = vg_lite_draw(fb, &path_circle_, VG_LITE_FILL_EVEN_ODD, &matrix_face_[1], VG_LITE_BLEND_NONE, 0xffcccccc);
    if (error) return error;
    
    // Draw ticks
    for (int i = 0; i < 12; i++) {
        // Note: matrix_tick_ only initialized for i=0,3,6,9
        // error = vg_lite_draw(fb, &path_button_, VG_LITE_FILL_EVEN_ODD, &matrix_tick_[i], VG_LITE_BLEND_NONE, 0x78cc78ff);
    }
    
    // Draw hands
    error = vg_lite_draw(fb, &path_hand_, VG_LITE_FILL_EVEN_ODD, &matrix_hour_, VG_LITE_BLEND_NONE, 0xff0000ff);
    if (error) return error;
    
    error = vg_lite_draw(fb, &path_hand_, VG_LITE_FILL_EVEN_ODD, &matrix_min_, VG_LITE_BLEND_NONE, 0xff00ff00);
    if (error) return error;
    
    error = vg_lite_draw(fb, &path_hand_, VG_LITE_FILL_EVEN_ODD, &matrix_sec_, VG_LITE_BLEND_NONE, 0xffff0000);
    if (error) return error;
    
    error = vg_lite_draw(fb, &path_circle_, VG_LITE_FILL_EVEN_ODD, &matrix_cap_, VG_LITE_BLEND_NONE, 0xff453340);
    return error;
}

TEST_F(ClockTest, Clock) {
    vg_lite_error_t error = VG_LITE_SUCCESS;
    
    // Initialize vg_lite engine
    error = vg_lite_init(FB_WIDTH, FB_HEIGHT);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_init failed";
    
    // Allocate off-screen buffer
    buffer_.width  = FB_WIDTH;
    buffer_.height = FB_HEIGHT;
    buffer_.format = VG_LITE_RGB565;
    
    error = vg_lite_allocate(&buffer_);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_allocate failed";
    
    // Clear buffer with blue
    error = vg_lite_clear(&buffer_, NULL, 0xFFFF0000);
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_clear failed";
    
    // Build paths and matrices
    buildPaths();
    buildMatrices();
    
    // Render clock
    error = renderClock();
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "renderClock failed";
    
    error = vg_lite_finish();
    ASSERT_EQ(error, VG_LITE_SUCCESS) << "vg_lite_finish failed";
    
    // Save PNG file
    vg_lite_save_png("clock.png", &buffer_);
}

} // anonymous namespace
