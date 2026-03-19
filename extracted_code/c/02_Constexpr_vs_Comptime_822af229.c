/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 2231
 * Language: c
 * Block ID: 822af229
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 类型安全容器（有限）
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// 使用宏生成类型专用容器
#define DEFINE_VECTOR(T, Name) \
    typedef struct { \
        T* data; \
        size_t len; \
        size_t cap; \
    } Name; \
    \
    static inline int Name##_init(Name* v, size_t cap) { \
        v->data = malloc(cap * sizeof(T)); \
        if (!v->data) return -1; \
        v->len = 0; \
        v->cap = cap; \
        return 0; \
    } \
    \
    static inline void Name##_free(Name* v) { \
        free(v->data); \
        v->data = NULL; \
        v->len = v->cap = 0; \
    } \
    \
    static inline int Name##_push(Name* v, T val) { \
        if (v->len >= v->cap) { \
            size_t new_cap = v->cap * 2; \
            T* new_data = realloc(v->data, new_cap * sizeof(T)); \
            if (!new_data) return -1; \
            v->data = new_data; \
            v->cap = new_cap; \
        } \
        v->data[v->len++] = val; \
        return 0; \
    } \
    \
    static inline T Name##_get(Name* v, size_t idx) { \
        assert(idx < v->len); \
        return v->data[idx]; \
    }

// 实例化
DEFINE_VECTOR(int, IntVector)
DEFINE_VECTOR(double, DoubleVector)
DEFINE_VECTOR(void*, PtrVector)

// 使用
void example(void) {
    IntVector v;
    IntVector_init(&v, 10);
    IntVector_push(&v, 42);
    int x = IntVector_get(&v, 0);
    IntVector_free(&v);
}

// 局限性：
// - 需要为每种类型生成代码
// - 类型检查在宏展开后
// - 无法根据类型特性优化
