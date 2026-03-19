/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 431
 * Language: c
 * Block ID: d247666f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 错误: 多个线程写相邻变量 */
typedef struct {
    atomic_int counter1;  /* 线程1 */
    atomic_int counter2;  /* 线程2 */
} bad_counters_t;

/* 正确: 填充到不同缓存行 */
typedef struct {
    atomic_int counter;
    char padding[64 - sizeof(atomic_int)];
} padded_counter_t;

typedef struct {
    padded_counter_t counters[MAX_WORKERS];
} good_counters_t;
