/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 372
 * Language: c
 * Block ID: 85e077f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用宏实现泛型
#define DECLARE_VECTOR(TYPE) \
    typedef struct { \
        TYPE *data; \
        size_t size; \
        size_t capacity; \
    } Vector_##TYPE; \
    \
    Vector_##TYPE* vec_##TYPE##_create(void); \
    void vec_##TYPE##_push(Vector_##TYPE *v, TYPE val); \
    TYPE vec_##TYPE##_get(const Vector_##TYPE *v, size_t index); \
    // ...

// 实例化
DECLARE_VECTOR(int)
DECLARE_VECTOR(double)
DECLARE_VECTOR(char*)
