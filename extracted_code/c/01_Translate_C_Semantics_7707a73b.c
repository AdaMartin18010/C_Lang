/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 1461
 * Language: c
 * Block ID: 7707a73b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: graphics_types.h
#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

#include <stdint.h>

#ifdef _WIN32
    #define GFX_API __declspec(dllexport)
#else
    #define GFX_API __attribute__((visibility("default")))
#endif

/* 颜色表示 */
typedef union {
    uint32_t value;
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } components;
} Color;

/* 顶点格式 */
typedef struct __attribute__((packed)) Vertex {
    float x, y, z;
    float u, v;
    Color color;
} Vertex;

/* 变换矩阵 */
typedef struct {
    float m[4][4];
} Matrix4x4;

/* 渲染命令 */
typedef enum : uint8_t {
    CMD_DRAW_TRIANGLES = 0,
    CMD_DRAW_LINES = 1,
    CMD_CLEAR = 2,
    CMD_SET_VIEWPORT = 3,
    CMD_MAX
} RenderCommand;

/* 渲染状态 */
typedef struct {
    uint32_t flags;
    uint16_t viewport_x;
    uint16_t viewport_y;
    uint16_t viewport_width;
    uint16_t viewport_height;
    float clear_color[4];
    Matrix4x4 projection;
    Matrix4x4 view;
    Matrix4x4 model;
} RenderState;

/* 纹理描述 */
typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t mip_levels;
    const void* data;
} TextureDesc;

/* 图形上下文 (不透明) */
typedef struct GraphicsContext GraphicsContext;

GFX_API GraphicsContext* gfx_create_context(void);
GFX_API void gfx_destroy_context(GraphicsContext* ctx);
GFX_API int gfx_submit_command(GraphicsContext* ctx, RenderCommand cmd,
                                const void* data, size_t data_size);

#endif
