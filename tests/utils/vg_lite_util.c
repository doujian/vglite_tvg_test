/**
 * @file vg_lite_util.c
 * @brief VGLite Utilities - merged implementation with PNG support
 * 
 * This file combines:
 * - vg_lite_util.c (raw file I/O, YUV conversion)
 * - load_png.c (PNG loading)
 * - save_png.c (PNG saving)
 * - fbdev.c (framebuffer - Linux only)
 * 
 * Windows/Visual Studio compatible
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-specific includes */
#if defined(_WIN32)
    /* Windows-specific headers if needed */
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/mman.h>
    #include <sys/ioctl.h>
#endif

#include "vg_lite_util.h"
#include "libpng/png.h"

#if defined(GPU_CHIP_ID_GCNanoUltraV)
static vg_lite_float_t _calc_decnano_compress_ratio(
    vg_lite_buffer_format_t format,
    vg_lite_compress_mode_t compress_mode
    )
{
    vg_lite_float_t ratio = 1.0f;

    switch(compress_mode) {
    case VG_LITE_DEC_NON_SAMPLE:
        switch(format) {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
            ratio = 0.625;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.5;
            break;
        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
            ratio = 0.667;
            break;
        default:
            return ratio;
        }
        break;

    case VG_LITE_DEC_HSAMPLE:
        switch(format) {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
            ratio = 0.5;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.375;
            break;
        default:
            return ratio;
        }
        break;

    case VG_LITE_DEC_HV_SAMPLE:
        switch(format) {
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_RGBA8888:
            ratio = 0.375;
            break;
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_RGBX8888:
            ratio = 0.25;
            break;
        default:
            return ratio;
        }
        break;
    default:
        return ratio;
    }
    return ratio;
}
#endif

static int write_int(FILE *fp,int  l)
{
    putc(l, fp);
    return (putc(l >> 8, fp));
}

static int read_int(FILE *fp)
{
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);

    return ((int)b0 | b1 << 8 );
}

static int read_int_inverse(FILE *fp)
{
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);

    return ((int)b1 | b0 << 8 );
}

/* Read a 32-bit signed integer. */
static int read_long(FILE *fp)
{
    unsigned char b0, b1, b2, b3; /* Bytes from file */
    
    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);
    
    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}

/* Write a 32-bit signed integer. */
static int write_long(FILE *fp,int  l)
{
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
}

/* ============================================================================
 * PNG Loading/Saving Functions
 * ============================================================================ */

int vg_lite_load_png(vg_lite_buffer_t * buffer, const char * name)
{
    /* Set status. */
    int status = 0;
    png_image image;

    for (;;) {
        /* Zero the buffer structure. */
        memset(buffer, 0, sizeof(vg_lite_buffer_t));

        /* Construct the PNG image structure. */
        memset(&image, 0, (sizeof image));
        image.version = PNG_IMAGE_VERSION;

        /* Read the PNG header. */
        if (!png_image_begin_read_from_file(&image, name))
            break;

        /* Convert PNG format into VGLite format. */
        if (image.format == PNG_FORMAT_GRAY) {
            /* Set to L8 format. */
            buffer->format = VG_LITE_L8;
        } else {
            /* Set to RGBA8888 format. */
            buffer->format = VG_LITE_RGBA8888;
            image.format = PNG_FORMAT_RGBA;
        }

        /* Set VGLite buffer width and height. */
        buffer->width  = image.width;
        buffer->height = image.height;

        /* Allocate the VGLite buffer memory. */
        if (vg_lite_allocate(buffer) != 0)
            break;

        /* Read the PNG image. */
        if (!png_image_finish_read(&image, NULL, buffer->memory, buffer->stride, NULL))
            break;

        /* Success. */
        status = 1;
        break;
    }

    if (!status && (buffer->handle != NULL)) {
        /* Free the VGLite buffer memory. */
        vg_lite_free(buffer);
    }

    /* Return the status. */
    return status;
}

int vg_lite_save_png(const char *name, vg_lite_buffer_t *buffer)
{
    uint8_t *memory, *p, *q;
    int x, y;
    png_image image;
    int status;
    uint16_t color;

    if (buffer->format == VG_LITE_L8) {
        /* Construct the PNG image structure. */
        png_image image;
        memset(&image, 0, (sizeof image));

        image.version = PNG_IMAGE_VERSION;
        image.width   = buffer->width;
        image.height  = buffer->height;
        image.format  = (  buffer->format == VG_LITE_L8 ? PNG_FORMAT_GRAY
                         : buffer->format == VG_LITE_RGBA8888 ? PNG_FORMAT_ARGB
                         : PNG_FORMAT_ABGR);

        /* Write the PNG image. */
        return png_image_write_to_file(&image, name, 0, buffer->memory, buffer->stride, NULL);
    }

    /* Allocate RGB memory buffer. */
    memory = malloc(buffer->width * buffer->height * 3);
    if (memory == NULL) {
        return 0;
    }

    for (y = 0; y < buffer->height; y++) {
        p = (uint8_t*) buffer->memory + y * buffer->stride;
        q = memory + y * buffer->width * 3;
        for (x = 0; x < buffer->width; x++, q += 3) {
            switch (buffer->format) {
#if defined(GPU_CHIP_ID_GCNanoUltraV)
                case VG_LITE_RGBA5658_PLANAR:
                case VG_LITE_ARGB8565_PLANAR:
#endif
                case VG_LITE_RGB565:
                    color = *(uint16_t *)p;
                    p += 2;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

#if defined(GPU_CHIP_ID_GCNanoUltraV)
                case VG_LITE_ABGR8565_PLANAR:
                case VG_LITE_BGRA5658_PLANAR:
#endif
                case VG_LITE_BGR565:
                    color = *(uint16_t *)p;
                    p += 2;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

#if defined(GPU_CHIP_ID_GCNanoUltraV)
                case VG_LITE_ABGR8565:
                    p += 1;
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 2;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_BGRA5658:
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 3;
                    q[0] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ARGB8565:
                    p += 1;
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 2;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

                case VG_LITE_RGBA5658:
                    color = *(uint8_t *)p | *(uint8_t *)(p + 1) << 8;
                    p += 3;
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    q[1] = ((color & 0x07E0) >> 3) | ((color & 0x0600) >> 9);
                    q[2] = ((color & 0xF800) >> 8) | ((color & 0xE000) >> 13);
                    break;

                case VG_LITE_RGB888:
                    q[0] = p[0];
                    q[1] = p[1];
                    q[2] = p[2];
                    p += 3;
                    break;

                case VG_LITE_BGR888:
                    q[0] = p[2];
                    q[1] = p[1];
                    q[2] = p[0];
                    p += 3;
                    break;
#endif

                case VG_LITE_RGBA8888:
                case VG_LITE_RGBX8888:
                    q[0] = p[0];
                    q[1] = p[1];
                    q[2] = p[2];
                    p += 4;
                    break;

                case VG_LITE_ARGB8888:
                case VG_LITE_XRGB8888:
                    q[0] = p[1];
                    q[1] = p[2];
                    q[2] = p[3];
                    p += 4;
                    break;

                case VG_LITE_BGRA8888:
                case VG_LITE_BGRX8888:
                    q[0] = p[2];
                    q[1] = p[1];
                    q[2] = p[0];
                    p += 4;
                    break;

                case VG_LITE_RGBA4444:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[0] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[2] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_BGRA4444:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[2] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[0] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_ABGR4444:
                    color = *(uint16_t*)p;
                    color = (color >> 4);
                    p += 2;
                    q[2] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[0] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_ARGB4444:
                    color = *(uint16_t*)p;
                    color = (color >> 4);
                    p += 2;
                    q[0] = (color & 0x000F) << 4;
                    q[1] = (color & 0x00F0);
                    q[2] = (color & 0x0F00) >> 4;
                    break;

                case VG_LITE_BGRA5551:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[0] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ABGR1555:
                    color = *(uint16_t*)p;
                    color = (color >> 1);
                    p += 2;
                    q[0] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[2] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_RGBA5551:
                    color = *(uint16_t*)p;
                    p += 2;
                    q[2] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_ARGB1555:
                    color = *(uint16_t*)p;
                    color = (color >> 1);
                    p += 2;
                    q[2] = ((color & 0x7C00) >> 7) | ((color & 0x7000) >> 12);
                    q[1] = ((color & 0x03E0) >> 2) | ((color & 0x0380) >> 7);
                    q[0] = ((color & 0x001F) << 3) | ((color & 0x001C) >> 2);
                    break;

                case VG_LITE_RGBA2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[0] = (color & 0x03) << 2 | (color & 0x03) << 4 | (color & 0x03) << 6 |(color & 0x03);
                    q[1] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[2] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    break;
                    
                case VG_LITE_BGRA2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[2] = (color & 0x03) << 2 | (color & 0x03) << 4 | (color & 0x03) << 6 |(color & 0x03);
                    q[1] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[0] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    break;

                case VG_LITE_ARGB2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[0] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[1] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    q[2] = (color & 0xC0) << 2 | (color & 0xC0) << 4 | (color & 0xC0) << 6 |(color & 0xC0);
                    break;

                case VG_LITE_ABGR2222:
                    color = *(uint8_t*)p;
                    p += 1;
                    q[2] = (color & 0x0C) << 2 | (color & 0x0C) << 4 | (color & 0x0C) << 6 |(color & 0x0C);
                    q[1] = (color & 0x30) << 2 | (color & 0x30) << 4 | (color & 0x30) << 6 |(color & 0x30);
                    q[0] = (color & 0xC0) << 2 | (color & 0xC0) << 4 | (color & 0xC0) << 6 |(color & 0xC0);
                    break;

                case VG_LITE_A8:
                case VG_LITE_L8:
                    q[0] = q[1] = q[2] = p[0];
                    p++;
                    break;

                case VG_LITE_YUYV:
                    /* YUYV not supported yet. */

                default:
                    break;
            }
        }
    }

    /* Construct the PNG image structure. */
    memset(&image, 0, (sizeof image));

    image.version = PNG_IMAGE_VERSION;
    image.width   = buffer->width;
    image.height  = buffer->height;
    image.format  = PNG_FORMAT_RGB;

    /* Write the PNG image. */
    status = png_image_write_to_file(&image, name, 0, memory, buffer->width * 3, NULL);

    /* Free the RGB memory buffer.*/
    free(memory);

    /* Success. */
    return status;
}

/* ============================================================================
 * Framebuffer Functions (Linux only)
 * ============================================================================ */

#ifdef HAVE_FBDEV
#include <linux/fb.h>

static int s_file;
static vg_lite_buffer_t s_framebuffer;
#endif

vg_lite_buffer_t * vg_lite_fb_open(void)
{
#ifdef HAVE_FBDEV
    struct fb_fix_screeninfo fix_info;
    struct fb_var_screeninfo var_info;

    /* Open /dev/fb0 device. */
    s_file = open("/dev/fb0", O_RDWR);
    if (s_file == -1) {
        return NULL;
    }

    /* Get the fixed fbdev info. */
    if (ioctl(s_file, FBIOGET_FSCREENINFO, &fix_info) < 0) {
        close(s_file);
        return NULL;
    }

    /* Get the variable fbdev info. */
    if (ioctl(s_file, FBIOGET_VSCREENINFO, &var_info) < 0) {
        close(s_file);
        return NULL;
    }

    switch (var_info.bits_per_pixel) {
        case 16:
            s_framebuffer.format = var_info.red.offset ? VG_LITE_BGR565 : VG_LITE_RGB565;
            break;

        case 32:
            s_framebuffer.format = var_info.red.offset ? VG_LITE_BGRA8888 : VG_LITE_RGBA8888;
            break;

        default:
            close(s_file);
            return NULL;
    }

    /* Fill in framebuffer info. */
    s_framebuffer.width = var_info.xres;
    s_framebuffer.height = var_info.yres;
    s_framebuffer.stride = fix_info.line_length;
    s_framebuffer.address = fix_info.smem_start;

    vg_lite_map_flag_t flag = VG_LITE_MAP_USER_MEMORY;
    int32_t fd = -1;
    if (vg_lite_map(&s_framebuffer, flag, fd) != VG_LITE_SUCCESS) {
        close(s_file);
        return NULL;
    }

    s_framebuffer.memory = mmap(NULL, s_framebuffer.stride * s_framebuffer.height, PROT_READ | PROT_WRITE, MAP_SHARED,
                                s_file, 0);
    if (s_framebuffer.memory == NULL) {
        vg_lite_unmap(&s_framebuffer);
        close(s_file);
    }

    return &s_framebuffer;
#else
    return NULL;
#endif
}

void vg_lite_fb_close(vg_lite_buffer_t * buffer)
{
#ifdef HAVE_FBDEV
    munmap(buffer->memory, buffer->stride * buffer->height);
    vg_lite_unmap(buffer);
    close(s_file);
#endif
}

/* ============================================================================
 * YUV Conversion Functions
 * ============================================================================ */

int vg_lite_yv12toyv24(vg_lite_buffer_t * buffer1, vg_lite_buffer_t * buffer2)
{
    int status = 0;
    int i=0;
    int j=0;
    int width = buffer2->width;
    int height = buffer2->height;
    buffer1->width = buffer2->width;
    buffer1->height = buffer2->height;
    buffer1->format = VG_LITE_YV24;

    if(buffer2->format != VG_LITE_YV12) {
        return -1;
    }

    vg_lite_allocate(buffer1);

    for( j=0;j<height;j++) {
        for( i=0;i<width;i++) {
            *((uint8_t*)buffer1->memory + j*width + i) = *((uint8_t*)buffer2->memory + j*width + i);
        }
    }

    for( j=0;j<height;j++) {
        for( i=0;i<width;i++) {
            *((uint8_t*)buffer1->yuv.uv_memory +j*width + i) = *((uint8_t*)buffer2->yuv.uv_memory + (j>>1)*width/2 + (i>>1));
        }
    }

    for( j=0;j<height;j++) {
        for( i=0;i<width;i++) {
            *((uint8_t*)buffer1->yuv.v_memory + j*width + i) = *((uint8_t*)buffer2->yuv.v_memory + (j>>1)*width/2 + (i>>1));
        }
    }

    status = 0;
    return status;
}

int vg_lite_yv12toyv16(vg_lite_buffer_t * buffer1, vg_lite_buffer_t * buffer2)
{
    int status = 0;
    int i=0;
    int j=0;
    int width  = buffer2->width;
    int height = buffer2->height;
    buffer1->width = buffer2->width;
    buffer1->height = buffer2->height;
    buffer1->format = VG_LITE_YV16;

    if(buffer2->format != VG_LITE_YV12) {
        return -1;
    }

    vg_lite_allocate(buffer1);

    for( j=0;j<height;j++) {
        for( i=0;i<width;i++) {
            *((uint8_t*)buffer1->memory + j*width + i) = *((uint8_t*)buffer2->memory + j*width + i);
        }
    }

    for( j=0;j<height;j++) {
        for( i=0;i<width/2;i++) {
            *((uint8_t*)buffer1->yuv.uv_memory + j*width/2 + i) = *((uint8_t*)buffer2->yuv.uv_memory + (j>>1)*width/2 + i);
        }
    }

    for(j=0; j<height; j++) {
        for(i=0; i<width/2; i++) {
            *((uint8_t*)buffer1->yuv.v_memory + j*width/2 + i) = *((uint8_t*)buffer2->yuv.v_memory + (j>>1)*width/2 + i);
        }
    }

    status = 0;
    return status;
}

int vg_lite_load_raw_yuv(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;
    uint8_t* p = NULL;
    uint32_t pos_line;
    int status = 0;
    int i,j,k;
    int Y_height;
    int U_height;
    int V_height;
    int Y_width;
    int U_width;
    int V_width;
    int width = buffer->width;
    int height = buffer->height;

    if(buffer->width == 0 || buffer->height == 0) {
        return -1;
    }

    switch (buffer->format) {
        case VG_LITE_NV12:
            Y_height=height;
            U_height=height/2;
            V_height=0;
            Y_width=width;
            U_width=width;
            V_width=0;
            break;

        case VG_LITE_YV12:
            Y_height=height;
            U_height=height/2;
            V_height=height/2;
            Y_width=width;
            U_width=width/2;
            V_width=width/2;
            break;

       case VG_LITE_NV16:
            Y_height=height;
            U_height=height;
            V_height=0;
            Y_width=width;
            U_width=width;
            V_width=0;
            break;
       default:
            return -1;
    }

    fp = fopen(name, "rb");
    if (fp != NULL) {
        int flag;
        if (vg_lite_allocate(buffer) != VG_LITE_SUCCESS) {
            fclose(fp);
            return -1;
        }

        p = (uint8_t*) buffer->memory;
        pos_line = ftell(fp);
        for(i = 0; i < Y_height; i++) {
            pos_line = ftell(fp);
            flag = fread(p, 1, Y_width, fp);
            if(flag != Y_width) {
                printf("failed to read raw buffer data\n");
                fclose(fp);
                return -1;
            }
            p += buffer->stride;
            fseek(fp, pos_line + Y_width, SEEK_SET);
        }

        p = (uint8_t*) buffer->yuv.uv_memory;
        pos_line = ftell(fp);
        for(j = 0; j < U_height; j++) {
            pos_line = ftell(fp);
            flag = fread(p, 1, U_width, fp);
            if(flag != U_width) {
                printf("failed to read raw buffer data\n");
                fclose(fp);
                return -1;
            }
            p += buffer->yuv.uv_stride;
            fseek(fp, pos_line + U_width, SEEK_SET);
        }

        p = (uint8_t*) buffer->yuv.v_memory;
        pos_line = ftell(fp);
        for(k = 0; k < V_height; k++) {
            pos_line = ftell(fp);
            flag = fread(p, 1, V_width, fp);
            if(flag != V_width) {
                printf("failed to read raw buffer data\n");
                fclose(fp);
                return -1;
            }
            p += buffer->yuv.v_stride;
            fseek(fp, pos_line + V_width, SEEK_SET);
        }

        fclose(fp);
        fp = NULL;
        status = 0;
        return status;
    }

    return -1;
}

#if defined(GPU_CHIP_ID_GCNanoUltraV)
int vg_lite_load_pkm_info_to_buffer(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;

    int status = 1;

    if(buffer == NULL)
        return -1;

    fp = fopen(name, "rb");
    if (fp != NULL) {
        read_long(fp);
        read_long(fp);
        buffer->width = read_int_inverse(fp);
        buffer->height = read_int_inverse(fp);
        buffer->format = VG_LITE_RGBA8888_ETC2_EAC;
        buffer->tiled = VG_LITE_TILED;
        fclose(fp);
        fp = NULL;
        status = 0;
    }
    return status;
}

int vg_lite_load_pkm(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;

    int status = 1;

    if(buffer == NULL)
        return -1;

    fp = fopen(name, "rb");
    if (fp != NULL) {
        int flag;

        fseek(fp, 16, SEEK_SET);
        flag = fread(buffer->memory, buffer->stride * buffer->height, 1, fp);
        if(flag != 1) {
            printf("failed to read pkm buffer data\n");
            fclose(fp);
            return -1;
        }

        fclose(fp);
        fp = NULL;
        status = 0;
    }

    return status;
}
#endif

int vg_lite_load_raw(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;

    /* Set status. */
    int status = 1;
    int format;
    /* Check the result with golden. */
    fp = fopen(name, "rb");
    if (fp != NULL) {
        int flag;
        
        /* Get width, height, stride and format info. */
        buffer->width  = read_long(fp);
        buffer->height = read_long(fp);
        buffer->stride = read_long(fp);
        format = read_long(fp);

        switch (format) {
        case 0:
            buffer->format = VG_LITE_RGBA8888;
            break;
        default:
            buffer->format = VG_LITE_RGBA8888;
        }

        /* Allocate the VGLite buffer memory. */
        if (vg_lite_allocate(buffer) != VG_LITE_SUCCESS)
        {
            fclose(fp);
            return -1;
        }

        fseek(fp, 16, SEEK_SET);
        flag = fread(buffer->memory, buffer->stride * buffer->height, 1, fp);
        if(flag != 1) {
            printf("failed to read raw buffer data\n");
            fclose(fp);
            return -1;
        }
        
        fclose(fp);
        fp = NULL;
        status = 0;
    }
    
    /* Return the status. */
    return status;
}

int vg_lite_load_raw_byline(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;
    
    /* Set status. */
    int status = 1;
    
    /* Check the result with golden. */
    fp = fopen(name, "r");
    if (fp != NULL) {
        int flag;
        
        /* Get width, height, stride and format info. */
        buffer->width  = read_long(fp);
        buffer->height = read_long(fp);
        buffer->stride = read_long(fp);
        buffer->format = read_long(fp);
        
        /* Allocate the VGLite buffer memory. */
        if (vg_lite_allocate(buffer) != VG_LITE_SUCCESS)
        {
            fclose(fp);
            return -1;
        }

        fseek(fp, 16, SEEK_SET);
        flag = fread(buffer->memory, buffer->width * buffer->height * (buffer->stride / buffer->width), 1, fp);
        if(flag != 1) {
            printf("failed to read raw buffer data\n");
            fclose(fp);
            return -1;
        }
        
        fclose(fp);
        fp = NULL;
        status = 0;
    }
    
    /* Return the status. */
    return status;
}

#if defined(GPU_CHIP_ID_GCNanoUltraV)
int vg_lite_load_dev_info_to_buffer(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;

    int status = 1;

    if(buffer == NULL)
        return -1;

    fp = fopen(name, "rb");
    if (fp != NULL) {
        buffer->width  = read_long(fp);
        buffer->height = read_long(fp);
        buffer->stride = read_long(fp);
        buffer->compress_mode = read_int(fp);
        buffer->format = read_int(fp);
        fclose(fp);
        fp = NULL;
        status = 0;
    }
    return status;
}

int vg_lite_load_decnano_compressd_data(vg_lite_buffer_t * buffer, const char * name)
{
    FILE * fp;
    int stride = 0;
    vg_lite_float_t ratio = 1.0f;
    int status = 1;
    fp = fopen(name, "rb");
    if (fp != NULL) {
        int flag;

        ratio = _calc_decnano_compress_ratio(buffer->format, buffer->compress_mode);
        stride = (int)(buffer->stride * ratio);
        fseek(fp, 16, SEEK_SET);
        flag = fread(buffer->memory, stride * buffer->height, 1, fp);
        if(flag != 1) {
            printf("failed to read raw buffer data\n");
            fclose(fp);
            return -1;
        }

        fclose(fp);
        fp = NULL;
        status = 0;
    }

    return status;
}

int vg_lite_save_decnano_compressd_data(const char *name, vg_lite_buffer_t *buffer)
{
    FILE * fp;
    int status = 1;
    int stride = 0;
    vg_lite_float_t ratio = 1.0f;
    fp = fopen(name, "wb");

    if (fp != NULL) {

        write_long(fp, buffer->width);
        write_long(fp, buffer->height);
        write_long(fp, buffer->stride);
        write_int(fp, buffer->compress_mode);
        write_int(fp, buffer->format);

        ratio = _calc_decnano_compress_ratio(buffer->format, buffer->compress_mode);
        stride = (int)(buffer->stride * ratio);

        fwrite(buffer->memory, 1, stride * buffer->height, fp);

        fclose(fp);
        fp = NULL;

        status = 0;
    }

    return status;
}
#endif

int vg_lite_save_raw(const char *name, vg_lite_buffer_t *buffer)
{
    FILE * fp;
    int status = 1;
    
    fp = fopen(name, "wb");
    
    if (fp != NULL) {
        /* Save width, height, stride and format info. */
        write_long(fp, buffer->width);
        write_long(fp, buffer->height);
        write_long(fp, buffer->stride);
        write_long(fp, buffer->format);
        
        /* Save buffer info. */
        fwrite(buffer->memory, 1, buffer->stride * buffer->height, fp);
        
        fclose(fp);
        fp = NULL;
        
        status = 0;
    }
    
    /* Return the status. */
    return status;
}

int vg_lite_save_raw_byline(const char *name, vg_lite_buffer_t *buffer)
{
    FILE * fp;
    int status = 1;
    unsigned char* pt;
    int loop, mem;
    
    fp = fopen(name, "w");
    
    if (fp != NULL) {
        /* Save width, height, stride and format info. */
        write_long(fp, buffer->width);
        write_long(fp, buffer->height);
        write_long(fp, buffer->stride);
        write_long(fp, buffer->format);
        
        /* Save buffer info. */
        pt = (unsigned char*) buffer->memory;
        for (loop = 0; loop < buffer->height; loop++) {
            for (mem = 0; mem < buffer->width * (buffer->stride / buffer->width); mem++) {
                putc(pt[mem], fp);
            }
        }
        
        fclose(fp);
        fp = NULL;
        
        status = 0;
    }
    
    /* Return the status. */
    return status;
}
