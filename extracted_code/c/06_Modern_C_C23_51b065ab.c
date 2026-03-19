/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 657
 * Language: c
 * Block ID: 51b065ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// vector.h - C23泛型向量
#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 泛型向量结构
typedef struct {
    void *data;
    size_t size;
    size_t capacity;
    size_t elem_size;
} Vector;

// 创建向量 (使用typeof实现泛型)
#define vector_create(T) \
    _vector_create(sizeof(T))

Vector _vector_create(size_t elem_size);

// 添加元素
#define vector_push(v, val) do { \
    typeof(val) _temp = (val); \
    _vector_push(v, &_temp); \
} while(0)

void _vector_push(Vector *v, const void *elem);

// 获取元素
#define vector_get(v, T, index) \
    (*(T*)_vector_get(v, index))

void *_vector_get(Vector *v, size_t index);

// 销毁
void vector_destroy(Vector *v);

#endif
