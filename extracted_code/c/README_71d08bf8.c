/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 1154
 * Language: c
 * Block ID: 71d08bf8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* renderer/renderer.c */
#include "renderer/renderer.h"
#include "renderer/command_buffer.h"
#include "ecs/components.h"

void renderer_render_scene(renderer_t* renderer, world_t* world, camera_t* camera) {
    rhi_context_t* rhi = renderer->rhi;
    command_buffer_t* cmd_buf = renderer->command_buffer;

    command_buffer_clear(cmd_buf);

    /* 更新相机 uniforms */
    float view_matrix[16], proj_matrix[16];
    camera_get_view_matrix(camera, view_matrix);
    camera_get_projection_matrix(camera, proj_matrix);

    rhi_begin_frame(rhi);

    /* 收集渲染命令 */
    component_type_t query[] = { COMPONENT_TRANSFORM, COMPONENT_RENDER };
    archetype_list_t* archetypes = world_query(world, query, 2);

    for (uint32_t a = 0; a < archetypes->count; a++) {
        archetype_t* arch = archetypes->items[a];

        ARCHETYPE_FOR_EACH(arch, chunk, i) {
            transform_component_t* transforms = archetype_get_component(
                arch, chunk, i, COMPONENT_TRANSFORM);
            render_component_t* renders = archetype_get_component(
                arch, chunk, i, COMPONENT_RENDER);

            for (uint32_t j = 0; j < chunk->count; j++) {
                if (!renders[j].visible) continue;

                mesh_t* mesh = resource_manager_get(renderer->resources,
                                                     renders[j].mesh_handle);
                material_t* material = resource_manager_get(renderer->resources,
                                                             renders[j].material_handle);

                /* 计算排序键：材质 -> 网格 -> 深度 */
                uint64_t sort_key = ((uint64_t)renders[j].material_handle << 32) |
                                    ((uint64_t)renders[j].mesh_handle << 16) |
                                    (uint64_t)(transforms[j].position[2] * 1000);

                /* 记录命令 */
                cmd_set_pipeline(cmd_buf, &material->pipeline_state, sort_key);
                cmd_set_vertex_buffer(cmd_buf, mesh->vertex_buffer,
                                      sizeof(vertex_t), sort_key);
                cmd_set_index_buffer(cmd_buf, mesh->index_buffer,
                                     RHI_FORMAT_R16_UINT, sort_key);

                /* 绑定材质参数 */
                float mvp[16];
                mat4_multiply(proj_matrix, view_matrix, mvp);
                mat4_multiply(mvp, transforms[j].world_matrix, mvp);

                /* 实例数据可以批量提交 */
                cmd_draw_indexed(cmd_buf, mesh->index_count, 0, sort_key);
            }
        }
    }

    /* 排序并执行命令，最小化状态切换 */
    command_buffer_sort(cmd_buf);
    command_buffer_execute(cmd_buf, rhi);

    rhi_end_frame(rhi);
}
