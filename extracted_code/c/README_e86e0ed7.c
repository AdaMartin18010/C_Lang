/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 528
 * Language: c
 * Block ID: e86e0ed7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* learning_path.c */
#include "skill_matrix.h"
#include "priority_matrix.h"
#include <stdlib.h>
#include <string.h>

/* 拓扑排序生成学习路径 */
skill_item_t **generate_learning_path(skill_matrix_t *matrix,
                                       uint32_t *path_length) {
    /* 使用Kahn算法进行拓扑排序 */
    uint32_t n = matrix->item_count;
    skill_item_t **path = calloc(n, sizeof(skill_item_t *));
    bool *visited = calloc(n, sizeof(bool));
    uint32_t *in_degree = calloc(n, sizeof(uint32_t));

    /* 计算入度 */
    for (uint32_t i = 0; i < n; i++) {
        skill_item_t *skill = &matrix->items[i];
        for (uint32_t j = 0; j < skill->prereq_count; j++) {
            /* 查找前置技能索引 */
            for (uint32_t k = 0; k < n; k++) {
                if (matrix->items[k].id == skill->prereq_ids[j]) {
                    in_degree[i]++;
                    break;
                }
            }
        }
    }

    /* Kahn算法主循环 */
    uint32_t path_idx = 0;
    bool progress = true;

    while (progress && path_idx < n) {
        progress = false;

        for (uint32_t i = 0; i < n; i++) {
            if (visited[i] || in_degree[i] > 0) continue;

            /* 选择当前可学习且优先级最高的 */
            uint32_t best_idx = i;
            for (uint32_t j = i + 1; j < n; j++) {
                if (!visited[j] && in_degree[j] == 0 &&
                    matrix->items[j].priority > matrix->items[best_idx].priority) {
                    best_idx = j;
                }
            }

            visited[best_idx] = true;
            path[path_idx++] = &matrix->items[best_idx];
            progress = true;

            /* 减少后继节点入度 */
            skill_item_t *completed = &matrix->items[best_idx];
            for (uint32_t j = 0; j < n; j++) {
                skill_item_t *skill = &matrix->items[j];
                for (uint32_t k = 0; k < skill->prereq_count; k++) {
                    if (skill->prereq_ids[k] == completed->id) {
                        in_degree[j]--;
                    }
                }
            }
        }
    }

    *path_length = path_idx;
    free(visited);
    free(in_degree);

    return path;
}

/* 输出学习路径报告 */
void print_learning_path(skill_item_t **path, uint32_t length) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    推荐学习路径                            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");

    uint32_t total_hours = 0;
    for (uint32_t i = 0; i < length; i++) {
        skill_item_t *skill = path[i];
        total_hours += skill->learning_hours;

        printf("║ %3d. %-40s [%s]\n",
               i + 1,
               skill->name,
               skill_level_string(skill->current_level));
        printf("║      目标: %-10s 优先级: %d  预计: %d小时\n",
               skill_level_string(skill->target_level),
               skill->priority,
               skill->learning_hours);
        printf("║\n");
    }

    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ 总计: %d项技能, 预计学习时长: %d小时 (~%.1f周)\n",
           length, total_hours, total_hours / 20.0);
    printf("╚════════════════════════════════════════════════════════════╝\n");
}
