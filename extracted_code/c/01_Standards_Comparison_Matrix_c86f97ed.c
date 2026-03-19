/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2799
 * Language: c
 * Block ID: c86f97ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "portable_defs.h"
#include <stdio.h>
#include <stdlib.h>

/* 使用条件编译处理不同标准 */
ATTR_NORETURN
void fatal_error(const char* msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(1);
}

/* 线程局部存储 */
thread_local int thread_id = 0;

/* 对齐的数据结构 */
typedef struct ATTR_ALIGNED(64) {
    char data[64];
} CacheLine;

/* 编译期断言 */
static_assert(sizeof(CacheLine) == 64, "Cache line size mismatch");

/* 原子操作包装 */
#if HAS_ATOMICS
    #include <stdatomic.h>
    typedef atomic_int atomic_counter_t;
    #define atomic_init_counter(c) atomic_init(c, 0)
    #define atomic_increment(c) atomic_fetch_add(c, 1)
#else
    typedef int atomic_counter_t;
    #define atomic_init_counter(c) (*(c) = 0)
    #define atomic_increment(c) (++(*(c)))
#endif

/* 多线程包装 */
#if HAS_THREADS
    #include <threads.h>
    typedef thrd_t thread_t;
    #define thread_create(th, fn, arg) thrd_create(th, fn, arg)
    #define thread_join(th, res) thrd_join(th, res)
#else
    /* 单线程存根 */
    typedef int thread_t;
    #define thread_create(th, fn, arg) 0
    #define thread_join(th, res) 0
#endif

ATTR_NODISCARD
int main(void) {
    printf("C Standard: %s\n",
           C23 ? "C23" : C17 ? "C17" : C11 ? "C11" : C99 ? "C99" : "C89");
    printf("Has atomics: %s\n", HAS_ATOMICS ? "yes" : "no");
    printf("Has threads: %s\n", HAS_THREADS ? "yes" : "no");

    atomic_counter_t counter;
    atomic_init_counter(&counter);
    atomic_increment(&counter);

    return 0;
}
