/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 1090
 * Language: c
 * Block ID: 9fe76253
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* renderer/command_buffer.h */
#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "rhi.h"

typedef enum {
    CMD_SET_PIPELINE,
    CMD_SET_VERTEX_BUFFER,
    CMD_SET_INDEX_BUFFER,
    CMD_DRAW,
    CMD_DRAW_INDEXED,
    CMD_SET_VIEWPORT,
    CMD_SET_SCISSOR,
    CMD_BIND_UNIFORM,
    CMD_BIND_TEXTURE
} render_command_type_t;

/* 渲染命令 - 紧凑布局，便于缓存 */
typedef struct {
    render_command_type_t type;
    union {
        struct { uint32_t vertex_count; uint32_t first_vertex; } draw;
        struct { uint32_t index_count; uint32_t first_index; } draw_indexed;
        struct { rhi_buffer_t buffer; uint32_t stride; } vertex_buffer;
        struct { rhi_buffer_t buffer; rhi_format_t format; } index_buffer;
        struct { uint32_t slot; rhi_texture_t texture; } texture;
        rhi_pipeline_state_t pipeline;
    } data;
} render_command_t;

/* 命令缓冲区 */
typedef struct {
    render_command_t* commands;
    uint32_t capacity;
    uint32_t count;

    /* 排序键用于渲染状态排序，减少状态切换 */
    uint64_t* sort_keys;
} command_buffer_t;

command_buffer_t* command_buffer_create(uint32_t capacity);
void command_buffer_destroy(command_buffer_t* cmd_buf);
void command_buffer_clear(command_buffer_t* cmd_buf);

/* 记录命令 */
void cmd_set_pipeline(command_buffer_t* cmd_buf,
                      const rhi_pipeline_state_t* state,
                      uint64_t sort_key);
void cmd_draw(command_buffer_t* cmd_buf,
              uint32_t vertex_count,
              uint32_t first_vertex,
              uint64_t sort_key);

/* 排序和执行 */
void command_buffer_sort(command_buffer_t* cmd_buf);
void command_buffer_execute(command_buffer_t* cmd_buf, rhi_context_t* ctx);

#endif
