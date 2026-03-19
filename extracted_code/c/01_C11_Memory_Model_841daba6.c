/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 750
 * Language: c
 * Block ID: 841daba6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 错误: 原子变量紧邻，导致缓存行竞争 */
typedef struct {
    _Atomic(int) counter1;  /* 同一线程写入 */
    _Atomic(int) counter2;  /* 另一线程写入 */
} bad_counters_t;

/* 正确: 缓存行填充 */
#define CACHE_LINE_SIZE 64

typedef struct {
    _Atomic(int) counter;
    char padding[CACHE_LINE_SIZE - sizeof(_Atomic(int))];
} padded_counter_t;

typedef struct {
    padded_counter_t counter1;
    padded_counter_t counter2;
} good_counters_t;
