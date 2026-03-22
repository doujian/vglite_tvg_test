/**
 * @file vg_lite_stub.cpp
 * Minimal stub implementation of vg_lite functions for testing
 */

#include "vg_lite.h"
#include <cstring>
#include <cstdlib>

/* Simple global state */
static int32_t g_width = 0;
static int32_t g_height = 0;

vg_lite_error_t vg_lite_init(int32_t width, int32_t height)
{
    g_width = width;
    g_height = height;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_close(void)
{
    g_width = 0;
    g_height = 0;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t * buffer)
{
    if (!buffer || buffer->width <= 0 || buffer->height <= 0) {
        return VG_LITE_INVALID_ARGUMENT;
    }
    
    /* Calculate stride */
    int32_t stride = ((buffer->width * 4 + 3) / 4) * 4;
    buffer->stride = stride;
    
    /* Allocate memory */
    size_t size = stride * buffer->height;
    buffer->memory = malloc(size);
    if (!buffer->memory) {
        return VG_LITE_OUT_OF_MEMORY;
    }
    
    memset(buffer->memory, 0, size);
    buffer->handle = (void*)0x1;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_free(vg_lite_buffer_t * buffer)
{
    if (!buffer) return VG_LITE_INVALID_ARGUMENT;
    if (buffer->memory) {
        free(buffer->memory);
        buffer->memory = nullptr;
    }
    buffer->handle = nullptr;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_clear(vg_lite_buffer_t * target, vg_lite_rectangle_t * rect, vg_lite_color_t color)
{
    if (!target || !target->memory) return VG_LITE_INVALID_ARGUMENT;
    
    int32_t x = 0, y = 0, w = target->width, h = target->height;
    if (rect) {
        x = rect->x; y = rect->y; w = rect->width; h = rect->height;
    }
    
    uint8_t* row = (uint8_t*)target->memory + y * target->stride;
    for (int32_t dy = 0; dy < h; dy++) {
        uint32_t* pixels = (uint32_t*)(row + dy * target->stride);
        for (int32_t dx = 0; dx < w; dx++) {
            pixels[x + dx] = color;
        }
    }
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_finish(void) { return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_flush(void) { return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_CLUT(uint32_t count, const uint32_t * colors) { (void)count; (void)colors; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_scissor(vg_lite_rectangle_t * rect) { (void)rect; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_disable_scissor(void) { return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_stroke(vg_lite_cap_style_t cap, vg_lite_join_style_t join, float width) { (void)cap; (void)join; (void)width; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_blit(vg_lite_buffer_t * target, vg_lite_buffer_t * source, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_filter_t filter) { (void)target; (void)source; (void)matrix; (void)blend; (void)color; (void)filter; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t * dst, vg_lite_buffer_t * src0, vg_lite_buffer_t * src1, vg_lite_matrix_t * matrix, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_filter_t filter, int32_t sx, int32_t sy, uint32_t sw, uint32_t sh, int32_t dx, int32_t dy, uint32_t dw, uint32_t dh) { (void)dst; (void)src0; (void)src1; (void)matrix; (void)blend; (void)color; (void)filter; (void)sx; (void)sy; (void)sw; (void)sh; (void)dx; (void)dy; (void)dw; (void)dh; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t * target, vg_lite_buffer_t * source, int32_t sx, int32_t sy, uint32_t width, uint32_t height, int32_t dx, int32_t dy, vg_lite_blend_t blend, vg_lite_color_t color, vg_lite_filter_t filter) { (void)target; (void)source; (void)sx; (void)sy; (void)width; (void)height; (void)dx; (void)dy; (void)blend; (void)color; (void)filter; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_draw_path(vg_lite_path_t * path, vg_lite_matrix_t * matrix, vg_lite_fill_t fill, vg_lite_color_t color, vg_lite_blend_t blend, vg_lite_filter_t filter) { (void)path; (void)matrix; (void)fill; (void)color; (void)blend; (void)filter; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_init_linear_gradient(vg_lite_linear_gradient_t * grad, uint32_t count) { if(!grad) return VG_LITE_INVALID_ARGUMENT; memset(grad, 0, sizeof(*grad)); (void)count; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_linear_gradient(vg_lite_linear_gradient_t * grad, float x1, float y1, float x2, float y2, vg_lite_gradient_spreadmode_t spread, uint32_t count) { (void)grad; (void)x1; (void)y1; (void)x2; (void)y2; (void)spread; (void)count; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_linear_gradient_set_ramp(vg_lite_linear_gradient_t * grad, uint32_t index, float offset, vg_lite_color_t color) { (void)grad; (void)index; (void)offset; (void)color; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_init_radial_gradient(vg_lite_radial_gradient_t * grad, uint32_t count) { if(!grad) return VG_LITE_INVALID_ARGUMENT; memset(grad, 0, sizeof(*grad)); (void)count; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_radial_gradient(vg_lite_radial_gradient_t * grad, float cx, float cy, float radius, vg_lite_gradient_spreadmode_t spread, uint32_t count) { (void)grad; (void)cx; (void)cy; (void)radius; (void)spread; (void)count; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_radial_gradient_set_ramp(vg_lite_radial_gradient_t * grad, uint32_t index, float offset, vg_lite_color_t color) { (void)grad; (void)index; (void)offset; (void)color; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_terminate_linear_gradient(vg_lite_linear_gradient_t * grad) { if(grad) memset(grad, 0, sizeof(*grad)); return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_terminate_radial_gradient(vg_lite_radial_gradient_t * grad) { if(grad) memset(grad, 0, sizeof(*grad)); return VG_LITE_SUCCESS; }
vg_lite_path_t * vg_lite_create_path(vg_lite_path_format_t format, vg_lite_fill_t fill, float scale, float bias, uint8_t * data, uint32_t data_length) { (void)format; (void)fill; (void)scale; (void)bias; (void)data_length; vg_lite_path_t *path = (vg_lite_path_t*)malloc(sizeof(vg_lite_path_t)); if(path) memset(path, 0, sizeof(*path)); return path; }
vg_lite_error_t vg_lite_delete_path(vg_lite_path_t * path) { if(path) free(path); return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_update_path(vg_lite_path_t * path, uint8_t * data) { (void)path; (void)data; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_draw_path(vg_lite_path_t * path, vg_lite_matrix_t * matrix) { (void)path; (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_path_fill(vg_lite_path_t * path, vg_lite_linear_gradient_t * fill) { (void)path; (void)fill; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_path_fill_radial(vg_lite_path_t * path, vg_lite_radial_gradient_t * fill) { (void)path; (void)fill; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_path_stroke(vg_lite_path_t * path, vg_lite_linear_gradient_t * stroke, float stroke_width, vg_lite_cap_style_t cap_style, vg_lite_join_style_t join_style, float miter_limit) { (void)path; (void)stroke; (void)stroke_width; (void)cap_style; (void)join_style; (void)miter_limit; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_path_stroke_radial(vg_lite_path_t * path, vg_lite_radial_gradient_t * stroke, float stroke_width, vg_lite_cap_style_t cap_style, vg_lite_join_style_t join_style, float miter_limit) { (void)path; (void)stroke; (void)stroke_width; (void)cap_style; (void)join_style; (void)miter_limit; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_update_linear_gradient(vg_lite_linear_gradient_t * grad, vg_lite_buffer_t * buffer) { (void)grad; (void)buffer; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_update_radial_gradient(vg_lite_radial_gradient_t * grad, vg_lite_buffer_t * buffer) { (void)grad; (void)buffer; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_matrix(vg_lite_matrix_t * matrix) { (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_get_matrix(vg_lite_matrix_t * matrix) { (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_identity(vg_lite_matrix_t * matrix) { if(matrix) memset(matrix, 0, sizeof(*matrix)); return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_translate(float x, float y, vg_lite_matrix_t * matrix) { (void)x; (void)y; (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_scale(float sx, float sy, vg_lite_matrix_t * matrix) { (void)sx; (void)sy; (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_rotate(float degree, vg_lite_matrix_t * matrix) { (void)degree; (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_multiply(vg_lite_matrix_t * matrix, vg_lite_matrix_t * mult) { (void)matrix; (void)mult; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_inverse(vg_lite_matrix_t * matrix) { (void)matrix; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_ortho(vg_lite_matrix_t * matrix, float left, float right, float bottom, float top, float near, float far) { (void)matrix; (void)left; (void)right; (void)bottom; (void)top; (void)near; (void)far; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_enable_rectangle(vg_lite_boolean_t enable) { (void)enable; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_rectangle(vg_lite_int32_t x, vg_lite_int32_t y, vg_lite_int32_t width, vg_lite_int32_t height) { (void)x; (void)y; (void)width; (void)height; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_alpha_mode(vg_lite_alpha_t mode) { (void)mode; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_set_color(vg_lite_color_t * color, vg_lite_uint8_t r, vg_lite_uint8_t g, vg_lite_uint8_t b, vg_lite_uint8_t a) { if(color) *color = ((vg_lite_color_t)r << 24) | ((vg_lite_color_t)g << 16) | ((vg_lite_color_t)b << 8) | a; return VG_LITE_SUCCESS; }
vg_lite_error_t vg_lite_reset(void) { return VG_LITE_SUCCESS; }
const char * vg_lite_error_string(vg_lite_error_t error) { (void)error; return "Unknown error"; }
