/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\01_C_ABI_Basics.md
 * Line: 50
 * Language: c
 * Block ID: e4d17033
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// === mathlib.h ===
#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 基本类型映射测试
typedef struct {
    int32_t x;
    int32_t y;
} Point;

// 暴露函数
double compute_distance(const Point *p1, const Point *p2);

// 迭代器模式（不透明指针）
typedef struct Iterator Iterator;
Iterator* iterator_create(const int32_t *data, size_t len);
bool iterator_next(Iterator *it, int32_t *out_val);
void iterator_destroy(Iterator *it);

// 回调注册
typedef void (*DataCallback)(int32_t data, void *user_data);
void register_callback(DataCallback cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
