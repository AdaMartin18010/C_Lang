/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 964
 * Language: c
 * Block ID: 2abf9f4a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* renderer/rhi.h */
#ifndef RHI_H
#define RHI_H

#include <stdint.h>
#include <stdbool.h>

/* 渲染后端枚举 */
typedef enum {
    RHI_BACKEND_OPENGL,
    RHI_BACKEND_VULKAN,
    RHI_BACKEND_D3D11,
    RHI_BACKEND_D3D12,
    RHI_BACKEND_METAL
} rhi_backend_t;

/* 句柄类型 */
typedef uint32_t rhi_buffer_t;
typedef uint32_t rhi_texture_t;
typedef uint32_t rhi_shader_t;
typedef uint32_t rhi_pipeline_t;
typedef uint32_t rhi_framebuffer_t;

typedef struct rhi_context rhi_context_t;

/* 上下文创建 */
rhi_context_t* rhi_create_context(rhi_backend_t backend, void* window_handle);
void rhi_destroy_context(rhi_context_t* ctx);

/* 缓冲区 */
typedef enum {
    RHI_BUFFER_VERTEX,
    RHI_BUFFER_INDEX,
    RHI_BUFFER_UNIFORM,
    RHI_BUFFER_STORAGE
} rhi_buffer_type_t;

rhi_buffer_t rhi_create_buffer(rhi_context_t* ctx,
                                rhi_buffer_type_t type,
                                size_t size,
                                const void* data);
void rhi_destroy_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);
void* rhi_map_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);
void rhi_unmap_buffer(rhi_context_t* ctx, rhi_buffer_t buffer);

/* 纹理 */
typedef enum {
    RHI_TEXTURE_2D,
    RHI_TEXTURE_3D,
    RHI_TEXTURE_CUBE
} rhi_texture_type_t;

typedef enum {
    RHI_FORMAT_R8G8B8A8_UNORM,
    RHI_FORMAT_R32G32B32_FLOAT,
    RHI_FORMAT_R32G32B32A32_FLOAT,
    RHI_FORMAT_D24_UNORM_S8_UINT,
    RHI_FORMAT_BC1_UNORM  /* DXT1 */
} rhi_format_t;

rhi_texture_t rhi_create_texture(rhi_context_t* ctx,
                                  rhi_texture_type_t type,
                                  uint32_t width,
                                  uint32_t height,
                                  uint32_t depth,
                                  rhi_format_t format,
                                  uint32_t mip_levels);
void rhi_destroy_texture(rhi_context_t* ctx, rhi_texture_t texture);
void rhi_update_texture(rhi_context_t* ctx,
                        rhi_texture_t texture,
                        uint32_t mip_level,
                        const void* data,
                        size_t size);

/* 渲染状态 */
typedef enum {
    RHI_CULL_NONE,
    RHI_CULL_FRONT,
    RHI_CULL_BACK
} rhi_cull_mode_t;

typedef enum {
    RHI_COMPARE_LESS,
    RHI_COMPARE_LEQUAL,
    RHI_COMPARE_EQUAL,
    RHI_COMPARE_GREATER,
    RHI_COMPARE_ALWAYS
} rhi_compare_func_t;

typedef struct {
    rhi_cull_mode_t cull_mode;
    bool depth_test;
    rhi_compare_func_t depth_func;
    bool depth_write;
    bool blend_enable;
    uint32_t viewport_width;
    uint32_t viewport_height;
} rhi_pipeline_state_t;

/* 绘制命令 */
void rhi_begin_frame(rhi_context_t* ctx);
void rhi_end_frame(rhi_context_t* ctx);

void rhi_set_pipeline_state(rhi_context_t* ctx, const rhi_pipeline_state_t* state);
void rhi_set_vertex_buffer(rhi_context_t* ctx, rhi_buffer_t buffer, uint32_t stride);
void rhi_set_index_buffer(rhi_context_t* ctx, rhi_buffer_t buffer, rhi_format_t format);
void rhi_set_shader(rhi_context_t* ctx, rhi_shader_t shader);

void rhi_draw(rhi_context_t* ctx, uint32_t vertex_count, uint32_t first_vertex);
void rhi_draw_indexed(rhi_context_t* ctx, uint32_t index_count, uint32_t first_index);
void rhi_draw_instanced(rhi_context_t* ctx, uint32_t vertex_count,
                        uint32_t instance_count, uint32_t first_instance);

/* 统一变量 */
void rhi_set_uniform_float(rhi_context_t* ctx, const char* name, float value);
void rhi_set_uniform_vec3(rhi_context_t* ctx, const char* name, const float* value);
void rhi_set_uniform_mat4(rhi_context_t* ctx, const char* name, const float* value);
void rhi_set_uniform_buffer(rhi_context_t* ctx, uint32_t slot, rhi_buffer_t buffer);
void rhi_set_texture(rhi_context_t* ctx, uint32_t slot, rhi_texture_t texture);

#endif
