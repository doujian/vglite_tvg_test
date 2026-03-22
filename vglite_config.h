/**
 * @file vglite_config.h
 * @brief VGLite ThorVG configuration - replaces LVGL dependencies
 * 
 * This header provides standalone macros and definitions for VGLite ThorVG
 * without requiring LVGL framework dependencies.
 */

#ifndef VGLITE_CONFIG_H
#define VGLITE_CONFIG_H

#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================
 * Feature Configuration
 *===========================*/

/* Enable VGLite ThorVG */
#define VGLITE_USE_THORVG           1

/* Buffer address alignment (bytes) */
#define VGLITE_BUF_ADDR_ALIGN       64

/* Thread rendering support (0 = disabled, 1 = enabled) */
#define VGLITE_THREAD_RENDER        0

/* YUV format support (0 = disabled, 1 = enabled) */
#define VGLITE_YUV_SUPPORT          0

/* LVGL blend support (0 = disabled, 1 = enabled) */
#define VGLITE_LVGL_BLEND_SUPPORT   0

/* Linear gradient extension support (0 = disabled, 1 = enabled) */
#define VGLITE_LINEAR_GRADIENT_EXT  0

/* 16 pixels alignment (0 = disabled, 1 = enabled) */
#define VGLITE_16PIXELS_ALIGN       0

/*============================
 * Logging Macros
 *===========================*/

#define VGLITE_LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[VGLITE ERROR] " fmt "\n", ##__VA_ARGS__)

#define VGLITE_LOG_WARN(fmt, ...) \
    fprintf(stdout, "[VGLITE WARN] " fmt "\n", ##__VA_ARGS__)

#define VGLITE_LOG_INFO(fmt, ...) \
    fprintf(stdout, "[VGLITE INFO] " fmt "\n", ##__VA_ARGS__)

#define VGLITE_LOG_USER(fmt, ...) \
    fprintf(stdout, "[VGLITE USER] " fmt "\n", ##__VA_ARGS__)

#define VGLITE_LOG_TRACE(fmt, ...) \
    fprintf(stdout, "[VGLITE TRACE] " fmt "\n", ##__VA_ARGS__)

/*============================
 * Assertion Macros
 *===========================*/

#define VGLITE_ASSERT(expr)         assert(expr)
#define VGLITE_ASSERT_NULL(expr)    assert((expr) == NULL)
#define VGLITE_ASSERT_NOT_NULL(expr) assert((expr) != NULL)

/*============================
 * Utility Macros
 *===========================*/

/* Mark parameter as unused */
#define VGLITE_UNUSED(x)            ((void)(x))

/*============================
 * LVGL Compatibility Layer
 * (For backward compatibility during migration)
 *===========================*/

/* Feature flags - map to VGLITE config */
#define LV_USE_DRAW_VG_LITE                 1
#define LV_USE_VG_LITE_THORVG               1
#define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN    VGLITE_BUF_ADDR_ALIGN
#define LV_VG_LITE_THORVG_THREAD_RENDER     VGLITE_THREAD_RENDER
#define LV_VG_LITE_THORVG_YUV_SUPPORT       VGLITE_YUV_SUPPORT
#define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT VGLITE_LVGL_BLEND_SUPPORT
#define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT VGLITE_LINEAR_GRADIENT_EXT
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN    VGLITE_16PIXELS_ALIGN

/* Logging macros - map to VGLITE logging */
#define LV_LOG_ERROR         VGLITE_LOG_ERROR
#define LV_LOG_WARN          VGLITE_LOG_WARN
#define LV_LOG_INFO          VGLITE_LOG_INFO
#define LV_LOG_USER          VGLITE_LOG_USER
#define LV_LOG_TRACE         VGLITE_LOG_TRACE

/* Assertion macros - map to VGLITE assertions */
#define LV_ASSERT            VGLITE_ASSERT
#define LV_ASSERT_NULL       VGLITE_ASSERT_NULL
#define LV_ASSERT_NOT_NULL   VGLITE_ASSERT_NOT_NULL

/* Utility macros - map to VGLITE utilities */
#define VGLITE_UNUSED(x)            ((void)(x))

/* Memory operations */
#include <string.h>
#include <stdlib.h>

#define VGLITE_MEMZERO(dst, size)    memset((dst), 0, (size))
#define lv_memzero(dst, size)    VGLITE_MEMZERO((dst), (size))
#define lv_malloc_zeroed(size)   calloc(1, (size))
#define lv_free(ptr)             free((ptr))
#define lv_memcpy(dst, src, size) memcpy((dst), (src), (size))

/* Unused parameter macro */
#define LV_UNUSED(x)             ((void)(x))

#ifdef __cplusplus
}
#endif

#endif /* VGLITE_CONFIG_H */
