# DemoTemplate.cmake - Reusable demo registration template
# 
# Usage:
#   include(cmake/DemoTemplate.cmake)
#   add_demo(demo_name source1.cpp source2.cpp ...)

function(add_demo DEMO_NAME)
    # Collect source files
    set(DEMO_SOURCES ${ARGN})
    
    # Shared include directories
    set(COMMON_INCLUDES
        ${VGLITE_ROOT}
        ${VGLITE_ROOT}/libs/thorvg-1.0.2/inc
        ${VGLITE_ROOT}/tests/utils
    )
    
    # Shared compile definitions
    set(COMMON_DEFINES
        TVG_STATIC
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS
        NOMINMAX
    )
    
    # Parent library sources
    set(COMMON_PARENT_SOURCES
        ${VGLITE_ROOT}/vg_lite_tvg.cpp
        ${VGLITE_ROOT}/vg_lite_matrix.c
    )
    
    # ============================================================================
    # GL version - uses OpenGL FBO for rendering
    # ============================================================================
    add_executable(${DEMO_NAME}_gl ${DEMO_SOURCES})
    
    target_include_directories(${DEMO_NAME}_gl PRIVATE ${COMMON_INCLUDES})
    
    target_sources(${DEMO_NAME}_gl PRIVATE ${COMMON_PARENT_SOURCES})
    
    target_compile_definitions(${DEMO_NAME}_gl PRIVATE 
        ${COMMON_DEFINES}
        VGLITE_USE_GL_BACKEND
    )
    
    target_link_libraries(${DEMO_NAME}_gl PRIVATE
        thorvg_gl
        vg_lite_util
        png_static
        zlib_static
        glfw
        opengl32
    )
    
    message(STATUS "Created demo target: ${DEMO_NAME}_gl")
    
    # ============================================================================
    # SW version - uses software rendering + texture upload
    # ============================================================================
    add_executable(${DEMO_NAME}_sw ${DEMO_SOURCES})
    
    target_include_directories(${DEMO_NAME}_sw PRIVATE ${COMMON_INCLUDES})
    
    target_sources(${DEMO_NAME}_sw PRIVATE ${COMMON_PARENT_SOURCES})
    
    target_compile_definitions(${DEMO_NAME}_sw PRIVATE ${COMMON_DEFINES})
    
    target_link_libraries(${DEMO_NAME}_sw PRIVATE
        thorvg_sw
        vg_lite_util
        png_static
        zlib_static
        glfw
        opengl32
    )
    
    message(STATUS "Created demo target: ${DEMO_NAME}_sw")
endfunction()
