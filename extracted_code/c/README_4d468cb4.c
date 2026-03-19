/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\README.md
 * Line: 317
 * Language: c
 * Block ID: 4d468cb4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* graph_query.c - 知识图谱查询实现 */
#include "knowledge_graph.h"
#include <stdlib.h>
#include <string.h>

/* 查找所有前置依赖 (递归) */
void find_all_prerequisites_recursive(entity_t *target,
                                       bool *visited,
                                       entity_t **result,
                                       uint32_t *count,
                                       uint32_t max_count) {
    for (uint32_t i = 0; i < target->in_degree && *count < max_count; i++) {
        relation_t *rel = target->incoming[i];
        if (rel->type == REL_PREREQUISITE || rel->type == REL_CONTAINS) {
            entity_t *prereq = rel->from;
            if (!visited[prereq->id]) {
                visited[prereq->id] = true;
                result[(*count)++] = prereq;
                find_all_prerequisites_recursive(prereq, visited, result,
                                                  count, max_count);
            }
        }
    }
}

entity_t **kg_find_prerequisites(knowledge_graph_t *kg,
                                  entity_t *target,
                                  uint32_t *count) {
    entity_t **result = calloc(kg->entity_count, sizeof(entity_t *));
    bool *visited = calloc(kg->entity_capacity, sizeof(bool));
    *count = 0;

    find_all_prerequisites_recursive(target, visited, result, count,
                                      kg->entity_count);

    free(visited);
    return result;
}

/* 查找最短学习路径 (BFS) */
entity_t **kg_find_learning_path(knowledge_graph_t *kg,
                                  entity_t *from,
                                  entity_t *to,
                                  uint32_t *path_length) {
    /* BFS实现 */
    entity_t **queue = calloc(kg->entity_count, sizeof(entity_t *));
    entity_t **parent = calloc(kg->entity_count, sizeof(entity_t *));
    bool *visited = calloc(kg->entity_capacity, sizeof(bool));

    uint32_t front = 0, rear = 0;
    queue[rear++] = from;
    visited[from->id] = true;

    bool found = false;
    while (front < rear && !found) {
        entity_t *current = queue[front++];

        for (uint32_t i = 0; i < current->out_degree; i++) {
            relation_t *rel = current->outgoing[i];
            if (rel->type == REL_PREREQUISITE || rel->type == REL_RELATED) {
                entity_t *next = rel->to;
                if (!visited[next->id]) {
                    visited[next->id] = true;
                    parent[next->id] = current;
                    queue[rear++] = next;

                    if (next == to) {
                        found = true;
                        break;
                    }
                }
            }
        }
    }

    /* 重建路径 */
    entity_t **path = calloc(kg->entity_count, sizeof(entity_t *));
    *path_length = 0;

    if (found) {
        entity_t *current = to;
        while (current != NULL) {
            path[(*path_length)++] = current;
            current = parent[current->id];
        }
        /* 反转路径 */
        for (uint32_t i = 0; i < *path_length / 2; i++) {
            entity_t *tmp = path[i];
            path[i] = path[*path_length - 1 - i];
            path[*path_length - 1 - i] = tmp;
        }
    }

    free(queue);
    free(parent);
    free(visited);

    return path;
}
