/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
 * Line: 1852
 * Language: c
 * Block ID: 00723337
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 宏实现类型安全容器
#define DECLARE_VECTOR(T, NAME) \
    typedef struct { \
        T* data; \
        size_t len; \
        size_t cap; \
    } NAME; \
    \
    static inline NAME NAME##_new(void) { \
        return (NAME){NULL, 0, 0}; \
    } \
    \
    static inline void NAME##_push(NAME* v, T val) { \
        if (v->len >= v->cap) { \
            v->cap = v->cap ? v->cap * 2 : 8; \
            v->data = realloc(v->data, v->cap * sizeof(T)); \
        } \
        v->data[v->len++] = val; \
    }

DECLARE_VECTOR(int, IntVec)
DECLARE_VECTOR(double, DoubleVec)
