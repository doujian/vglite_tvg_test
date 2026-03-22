/**
 * @file config.h
 * @brief ThorVG 1.0.2 configuration header (generated for CMake build)
 */

#ifndef TVG_CONFIG_H
#define TVG_CONFIG_H

/* VGLite GL Backend Configuration */
#include "../../../vglite_gl_config.h"

/* ThorVG Version */
#define THORVG_VERSION_STRING "1.0.2"

/* Multi-Tasking */
#define THORVG_THREAD_SUPPORT 1

/* Engines */
#define THORVG_SW_RASTER_SUPPORT 1
#if VGLITE_USE_GL_BACKEND
    #define THORVG_GL_RASTER_SUPPORT 1
#else
    /* #undef THORVG_GL_RASTER_SUPPORT */
#endif
/* #undef THORVG_WG_RASTER_SUPPORT */

/* Partial Rendering */
/* #undef THORVG_PARTIAL_RENDER_SUPPORT */

/* Loaders */
#define THORVG_SVG_LOADER_SUPPORT 1
/* #undef THORVG_PNG_LOADER_SUPPORT */
/* #undef THORVG_JPG_LOADER_SUPPORT */
/* #undef THORVG_LOTTIE_LOADER_SUPPORT */
/* #undef THORVG_TTF_LOADER_SUPPORT */
/* #undef THORVG_WEBP_LOADER_SUPPORT */

/* Savers */
/* #undef THORVG_GIF_SAVER_SUPPORT */

#endif /* TVG_CONFIG_H */
