/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 103
 * Language: c
 * Block ID: 7ab7db1d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicArray;

// 基本操作
DynamicArray* da_create(size_t initial_capacity);
void da_destroy(DynamicArray *arr);
int da_push(DynamicArray *arr, int value);
int da_pop(DynamicArray *arr, int *value);
int da_get(DynamicArray *arr, size_t index, int *value);
int da_set(DynamicArray *arr, size_t index, int value);

// 高级操作
int da_insert(DynamicArray *arr, size_t index, int value);
int da_remove(DynamicArray *arr, size_t index);
void da_sort(DynamicArray *arr);
int da_find(DynamicArray *arr, int value);

// 内存管理
void da_shrink_to_fit(DynamicArray *arr);
int da_reserve(DynamicArray *arr, size_t new_capacity);

#endif
