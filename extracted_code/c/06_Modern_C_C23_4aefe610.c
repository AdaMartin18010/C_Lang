/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 705
 * Language: c
 * Block ID: 4aefe610
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// vector.c
#include "vector.h"
#include <stdckdint.h>
#include <stdbit.h>

[[nodiscard]]
Vector _vector_create(size_t elem_size) {
    return (Vector){
        .data = nullptr,
        .size = 0,
        .capacity = 0,
        .elem_size = elem_size
    };
}

void _vector_push(Vector *v, const void *elem) {
    if (v == nullptr) return;

    // 检查是否需要扩容
    if (v->size >= v->capacity) {
        size_t new_cap = v->capacity == 0 ? 4 : v->capacity * 2;

        // 安全检查
        size_t new_size;
        if (ckd_mul(&new_size, new_cap, v->elem_size)) {
            return;  // 溢出
        }

        void *new_data = realloc(v->data, new_size);
        if (new_data == nullptr) return;

        v->data = new_data;
        v->capacity = new_cap;
    }

    // 复制元素
    memcpy((char*)v->data + v->size * v->elem_size,
           elem, v->elem_size);
    v->size++;
}

[[nodiscard]]
void *_vector_get(Vector *v, size_t index) {
    if (v == nullptr || index >= v->size) {
        return nullptr;
    }
    return (char*)v->data + index * v->elem_size;
}

void vector_destroy(Vector *v) {
    if (v == nullptr) return;
    free(v->data);
    v->data = nullptr;
    v->size = 0;
    v->capacity = 0;
}
