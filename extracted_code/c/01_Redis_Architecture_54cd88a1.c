/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 1157
 * Language: c
 * Block ID: 54cd88a1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 特性使用

// a) 静态断言
_Static_assert(sizeof(redisObject) == 16, "redisObject size mismatch");

// b) 匿名结构和联合
typedef struct {
    union {
        struct { int16_t hi, lo; } i16;
        int32_t i32;
    } value;
} number;

// c) 对齐支持
_Alignas(64) char cache_line[64];

// d) 原子操作
#include <stdatomic.h>
_Atomic size_t used_memory = 0;
atomic_fetch_add(&used_memory, size);

// e) 线程局部存储
_Thread_local struct random_data tls_random_data;

// f) 泛型选择（C11）
#define abs(x) _Generic((x), \
    int: abs_int, \
    long: abs_long, \
    default: abs_double \
)(x)
